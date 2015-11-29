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
#include "protocol_tcp6.h"
#include "lib/logging.h"

ProtocolTCP6::ProtocolTCP6() : ProtocolTCP() {
}

ProtocolTCP6::ProtocolTCP6(int newSocket, socklen_t len, const struct sockaddr * addr) : ProtocolTCP(newSocket, len, addr, false) {
}

ProtocolTCP6::ProtocolTCP6(ProtocolTCP6 && other) : ProtocolTCP(std::move(other)) {
}

ProtocolTCP6::~ProtocolTCP6() {
}

std::unique_ptr<Protocol> ProtocolTCP6::waitForNewConnection() {
    struct sockaddr_in6 addr;
    socklen_t addrlen=sizeof(addr);
    std::unique_lock<std::mutex> lck(lock);
    if (state == ProtocolState::CLOSED || type != ProtocolType::SERVER) {
        return std::unique_ptr<Protocol>(nullptr);
    }
    int newSocket=::accept(socket, (sockaddr *)&addr, &addrlen );
    if (newSocket < 0) {
        return std::unique_ptr<Protocol>(nullptr);
    }
    return std::unique_ptr<Protocol>(new ProtocolTCP6(newSocket, addrlen, (const sockaddr *)&addr));
}
