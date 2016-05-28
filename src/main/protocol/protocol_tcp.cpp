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
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <WS2tcpip.h>
#endif
#include <thread>
#include "lib/logging.h"
#include "protocol_tcp.h"

ProtocolTCP::ProtocolTCP(const std::string & protocolName) : Protocol(protocolName) {
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
            updateBytesRead(numRead);
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
                updateBytesRead(numRead);
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
    if (numWritten > 0) {
        updateBytesWritten(numWritten);
    }
	return numWritten == data.size();
}

bool ProtocolTCP::listen(const Host & localHost, int backlog) {
	std::unique_lock<std::mutex> lck(lock);
	if (state != ProtocolState::CLOSED || type != ProtocolInstanceType::NONE) {
		return false;
	}
	this->host = localHost;
	struct protoent *pr = getprotobyname("tcp");
	socket = ::socket(localHost.getPreferredSocketDomain(), SOCK_STREAM, pr->p_proto);
    optval_t optval = 1;
	::setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	// setsockopt(socket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
	if (::bind(socket, localHost.getPreferredSockAddress(), localHost.getPreferedSockAddressLen()) == 0) {
		if (::listen(socket, backlog) == 0) {
			type = ProtocolInstanceType::SERVER;
			state = ProtocolState::OPEN;
			return true;
		}
	}
	return false;
}

bool ProtocolTCP::connect(const Host & localHost) {
	std::unique_lock<std::mutex> lck(lock);
	if (state != ProtocolState::CLOSED || type != ProtocolInstanceType::NONE) {
		return false;
	}
	this->host = localHost;
	struct protoent *pr = getprotobyname("tcp");
	socket = ::socket(localHost.getPreferredSocketDomain(), SOCK_STREAM, pr->p_proto);
	if (::connect(socket, localHost.getPreferredSockAddress(), localHost.getPreferedSockAddressLen()) == 0) {
		type = ProtocolInstanceType::CLIENT;
		state = ProtocolState::OPEN;
		return true;
	}
	return false;
}

ProtocolTCP::ProtocolTCP(int socket, socklen_t len, const struct sockaddr * addr, bool isIPV4, const std::string & protocolName) :
		Protocol(socket, len, addr, isIPV4, protocolName) {
}

ProtocolTCP::ProtocolTCP(ProtocolTCP && other) : Protocol(other.host, other.type, other.socket, other.state, other.name) {
	other.socket = -1;
	other.host = Host::ALL_INTERFACES6;
	other.type = ProtocolInstanceType::NONE;
	other.state = ProtocolState::CLOSED;
    other.name = "";
}
