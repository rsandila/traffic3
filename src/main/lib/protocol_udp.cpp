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
#include "protocol_udp.h"

ProtocolUDP::ProtocolUDP() : Protocol() {
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
    socklen_t addr_len = (host.getProtocolPreference() == Host::ProtocolPreference::IPV4)?sizeof(addr.addr4):sizeof(addr.addr6);
	long int numRead;
	if (type == ProtocolType::CLIENT) {
		numRead = ::recv(socket, &data[0], data.size(), (allowPartialRead) ? 0 : MSG_WAITALL);
	}
	else {
		numRead = ::recvfrom(socket, &data[0], data.size(), (allowPartialRead) ? 0 : MSG_WAITALL,
                             (struct sockaddr *)&addr, &addr_len);
	}
	LOG(DEBUG) << std::this_thread::get_id() << " read " << numRead << std::endl;
#ifdef _MSC_VER
	LOG_IF(numRead == -1, WARNING) << "Error code = " << std::hex << WSAGetLastError() << std::endl;
#else
    LOG_IF(numRead == -1, WARNING) << "Error code = " << std::hex << errno << std::endl;
#endif
	if (numRead < 0) {
		return false;
	}
	if (numRead > 0) {
		data.resize(numRead);
	}
	if (type == ProtocolType::CLIENT) {
		hostState = host;
	}
	else {
        hostState = Host(addr_len, (struct sockaddr *)&addr,
                         host.getProtocolPreference() == Host::ProtocolPreference::IPV4);
	}
	if (allowPartialRead) {
		return numRead > 0;
	}
	else {
		return (unsigned long)numRead == data.size();
	}
}

bool ProtocolUDP::write(const std::vector<char> & data, const Host & hostState) {
	std::unique_lock<std::mutex> lck(lock);
	if (state == ProtocolState::CLOSED || data.size() == 0) {
		return false;
	}
	unsigned long numWritten;
	if (type == ProtocolType::SERVER_CLIENT) {
		numWritten = ::sendto(socket, &data[0], data.size(), 0, hostState.getPreferredSockAddress(),
			hostState.getPreferedSockAddressLen());
	}
	else {
		numWritten = ::send(socket, &data[0], data.size(), 0);
	}
	LOG(DEBUG) << std::this_thread::get_id() << " wrote " << numWritten << std::endl;
#ifdef _MSC_VER
    LOG_IF(numWritten == -1, WARNING) << "Error code = " << std::hex << WSAGetLastError() << std::endl;
#else
    LOG_IF(numWritten == (unsigned long)-1, WARNING) << "Error code = " << std::hex << errno << std::endl;
#endif
	return numWritten == data.size();
}

ProtocolUDP::ProtocolUDP(int socket, socklen_t len, const struct sockaddr * addr, bool isIPV4) : Protocol(socket, len, addr, isIPV4) {
}

ProtocolUDP::ProtocolUDP(ProtocolUDP && other) : Protocol(other.host, other.type, other.socket, other.state) {
	other.socket = -1;
	other.host = Host::ALL_INTERFACES6;
	other.type = ProtocolType::NONE;
	other.state = ProtocolState::CLOSED;
}

bool ProtocolUDP::listen(const Host & localHost, const int backlog) {
    UNUSED(backlog);
    std::unique_lock<std::mutex> lck(lock);
    if (state != ProtocolState::CLOSED || type != ProtocolType::NONE) {
        return false;
    }
    this->host = localHost;
    struct protoent *pr = getprotobyname("udp");
    socket = ::socket(localHost.getPreferredSocketDomain(), SOCK_DGRAM, pr->p_proto);
    if (socket < 0) {
        return false;
    }
    char optval = 1;
    setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    if (::bind(socket, localHost.getPreferredSockAddress(), localHost.getPreferedSockAddressLen()) == 0) {
        type = ProtocolType::SERVER;
        state = ProtocolState::OPEN;
        return true;
    }
    return false;
}

bool ProtocolUDP::connect(const Host & localHost) {
    std::unique_lock<std::mutex> lck(lock);
    if (state != ProtocolState::CLOSED || type != ProtocolType::NONE) {
        return false;
    }
    this->host = localHost;
    struct protoent *pr = getprotobyname("udp");
    socket = ::socket(localHost.getPreferredSocketDomain(), SOCK_DGRAM, pr->p_proto);
    if (socket < 0) {
        return false;
    }
#ifdef _MSC_VER
    char optval = 1;
#else
    int optval = 1;
#endif
    setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    if (::connect(socket, localHost.getPreferredSockAddress(), localHost.getPreferedSockAddressLen()) < 0) {
#ifndef _MSC_VER
        ::close(socket);
#else
        ::closesocket(socket);
#endif
        return false;
    }
    type = ProtocolType::CLIENT;
    state = ProtocolState::OPEN;
    return true;
}
