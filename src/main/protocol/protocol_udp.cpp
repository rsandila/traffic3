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
#include "protocol_udp.h"

ProtocolUDP::ProtocolUDP(const std::string & protocolName) : Protocol(protocolName) {
}

ProtocolUDP::~ProtocolUDP() {
	close();
}

bool ProtocolUDP::read(std::vector<char> & data, bool allowPartialRead, Host & hostState) {
	std::unique_lock<std::mutex> lck(lock);
	if (state == ProtocolState::CLOSED) {
		return false;
	}
    union {
        struct sockaddr_in addr4;
        struct sockaddr_in6 addr6;
    } addr;
	socklen_t addr_len = (host.getProtocolPreference() == Host::ProtocolPreference::IPV4) ? sizeof(addr.addr4) : sizeof(addr.addr6);
	long int numRead = ::recvfrom(socket, &data[0], data.size(), 0, (struct sockaddr *)&addr, &addr_len);
	LOG(DEBUG) << std::this_thread::get_id() << " read " << numRead << std::endl;
	if (numRead < 0) {
		return false;
	}
	if (numRead > 0) {
		data.resize(numRead);
        updateBytesRead(numRead);
	}
	if (type == ProtocolInstanceType::CLIENT) {
		hostState = host;
	} else {
        hostState = Host(addr_len, (struct sockaddr *)&addr,
                         host.getProtocolPreference() == Host::ProtocolPreference::IPV4);
	}
	if (allowPartialRead) {
		return numRead > 0;
	} else {
		return (unsigned long)numRead == data.size();
	}
}

bool ProtocolUDP::write(const std::vector<char> & data, const Host & hostState) {
	std::unique_lock<std::mutex> lck(lock);
	if (state == ProtocolState::CLOSED || data.size() == 0) {
		return false;
	}
	unsigned long numWritten;
	Host targetHost = (hostState == Host::ALL_INTERFACES4 || hostState == Host::ALL_INTERFACES6) ? host : hostState;
	numWritten = ::sendto(socket, &data[0], data.size(), 0, targetHost.getPreferredSockAddress(),
		targetHost.getPreferedSockAddressLen());
	LOG(DEBUG) << std::this_thread::get_id() << " wrote " << numWritten << std::endl;
    if (numWritten > 0) {
        updateBytesWritten(numWritten);
    }
	return numWritten == data.size();
}

ProtocolUDP::ProtocolUDP(int socket, socklen_t len, const struct sockaddr * addr, bool isIPV4, const std::string & protocolName) :
        Protocol(socket, len, addr, isIPV4, protocolName) {
}

ProtocolUDP::ProtocolUDP(ProtocolUDP && other) : Protocol(other.host, other.type, other.socket, other.state, other.name) {
	other.socket = -1;
	other.host = Host::ALL_INTERFACES6;
	other.type = ProtocolInstanceType::NONE;
	other.state = ProtocolState::CLOSED;
    other.name = "";
}

bool ProtocolUDP::listen(const Host & localHost, const int backlog) {
    UNUSED(backlog);
    std::unique_lock<std::mutex> lck(lock);
    if (state != ProtocolState::CLOSED || type != ProtocolInstanceType::NONE) {
        return false;
    }
    this->host = localHost;
    struct protoent *pr = getprotobyname("udp");
    socket = ::socket(localHost.getPreferredSocketDomain(), SOCK_DGRAM, pr->p_proto);
    if (socket < 0) {
        return false;
    }
    optval_t optval = 1;
    setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    if (::bind(socket, localHost.getPreferredSockAddress(), localHost.getPreferedSockAddressLen()) == 0) {
        type = ProtocolInstanceType::SERVER;
        state = ProtocolState::OPEN;
        return true;
    }
    return false;
}

bool ProtocolUDP::connect(const Host & localHost) {
    std::unique_lock<std::mutex> lck(lock);
    if (state != ProtocolState::CLOSED || type != ProtocolInstanceType::NONE) {
        return false;
    }
    this->host = localHost;
    struct protoent *pr = getprotobyname("udp");
    socket = ::socket(localHost.getPreferredSocketDomain(), SOCK_DGRAM, pr->p_proto);
    if (socket < 0) {
        return false;
    }
    optval_t optval = 1;
    setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	Host bindHost = Host::ALL_INTERFACES4;
	if (localHost.getProtocolPreference() == Host::ProtocolPreference::IPV6) {
		bindHost = Host::ALL_INTERFACES6;
	}
	if (::bind(socket, bindHost.getPreferredSockAddress(), bindHost.getPreferedSockAddressLen()) != 0) {
#ifdef _MSC_VER
        ::closesocket(socket);
#else
        ::close(socket);
#endif
		return false;
	}
	type = ProtocolInstanceType::CLIENT;
    state = ProtocolState::OPEN;
    return true;
}
