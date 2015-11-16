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
#include "protocol_udp4.h"
#include "logging.h"

ProtocolUDP4::ProtocolUDP4() : ProtocolUDP(), numConnections(0) {
}

ProtocolUDP4::ProtocolUDP4(int newSocket, socklen_t len, const struct sockaddr * addr) : ProtocolUDP(newSocket, len, addr, true) {
}

ProtocolUDP4::ProtocolUDP4(ProtocolUDP4 && other) : ProtocolUDP(std::move(other)), numConnections
        (other.numConnections.exchange(0)) {
}

ProtocolUDP4::~ProtocolUDP4() {
}

std::unique_ptr<Protocol> ProtocolUDP4::waitForNewConnection() {
    if (numConnections == 0) {
        numConnections++;
        std::unique_ptr<ProtocolUDP4> returnValue(new ProtocolUDP4(socket, host.getPreferedSockAddressLen(), host.getPreferredSockAddress()));
        return std::move(returnValue);
    }
    while (state != ProtocolState::CLOSED) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return std::unique_ptr<Protocol>(nullptr);
}

