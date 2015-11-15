/*
* Copyright (C) 2015 Robert Sandilands
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
USA.
*/
#ifndef _MSC_VER
#include <sys/socket.h>
#include <poll.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#else
#include <WinSock2.h>
#include <WS2tcpip.h>
#endif
#include <thread>
#include "logging.h"
#include "protocol_tcp.h"

ProtocolTCP::ProtocolTCP() : Protocol() {
}

ProtocolTCP::~ProtocolTCP() {
	close();
}

bool ProtocolTCP::read(std::vector<char> & data, bool allowPartialRead, Host & hostState) {
	UNUSED(hostState);
	std::unique_lock<std::mutex> lck(lock);
	if (state == ProtocolState::CLOSED) {
		return false;
	}
	if (allowPartialRead) {
		long int numRead = ::recv(socket, &data[0], data.size(), 0);
		if (numRead > 0) {
			data.resize(numRead);
		}
		return numRead > 0;
	}
	else {
		unsigned long offset = 0;
		unsigned long numRead;
		do {
			numRead = ::recv(socket, &data[offset], data.size() - offset, MSG_WAITALL);
			if (numRead > 0) {
				offset += numRead;
			}
			LOG(DEBUG) << std::this_thread::get_id() << " read " << numRead << std::endl;
		} while (numRead > 0 && offset < data.size());
		return offset == data.size();
	}
}

bool ProtocolTCP::write(const std::vector<char> & data, const Host & hostState) {
	UNUSED(hostState);
	std::unique_lock<std::mutex> lck(lock);
	if (state == ProtocolState::CLOSED || data.size() == 0) {
		return false;
	}
	unsigned long numWritten = ::send(socket, &data[0], data.size(), 0);
	return numWritten == data.size();
}

bool ProtocolTCP::realListen(const Host & localHost, const int af, const struct sockaddr * addr, const int addr_len, const int backlog) {
	std::unique_lock<std::mutex> lck(lock);
	if (state != ProtocolState::CLOSED || type != ProtocolType::NONE) {
		return false;
	}
	this->host = localHost;
	struct protoent *pr = getprotobyname("tcp");
	socket = ::socket(af, SOCK_STREAM, pr->p_proto);
#ifndef _MSC_VER
	int optval = 1;
#else
	char optval = 1;
#endif
	::setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	// setsockopt(socket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
	if (::bind(socket, addr, addr_len) == 0) {
		if (::listen(socket, backlog) == 0) {
			type = ProtocolType::SERVER;
			state = ProtocolState::OPEN;
			return true;
		}
	}
	return false;
}

bool ProtocolTCP::realConnect(const Host & localHost, const int af, const struct sockaddr * addr, const int addr_len) {
	std::unique_lock<std::mutex> lck(lock);
	if (state != ProtocolState::CLOSED || type != ProtocolType::NONE) {
		return false;
	}
	this->host = localHost;
	struct protoent *pr = getprotobyname("tcp");
	socket = ::socket(af, SOCK_STREAM, pr->p_proto);
	if (::connect(socket, addr, addr_len) == 0) {
		type = ProtocolType::CLIENT;
		state = ProtocolState::OPEN;
		return true;
	}
	return false;
}

ProtocolTCP::ProtocolTCP(int socket, socklen_t len, const struct sockaddr * addr, bool isIPV4) :
		Protocol(socket, len, addr, isIPV4) {
}

ProtocolTCP::ProtocolTCP(ProtocolTCP && other) : Protocol(other.host, other.type, other.socket, other.state) {
	other.socket = -1;
	other.host = Host::ALL_INTERFACES;
	other.type = ProtocolType::NONE;
	other.state = ProtocolState::CLOSED;
}


