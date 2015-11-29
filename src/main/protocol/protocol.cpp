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
#include <poll.h>
#include <unistd.h>
#else
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <WS2tcpip.h>
#endif
#include "protocol.h"

bool Protocol::isReady(const ProtocolState & expected, int timeoutInMilliseconds) {
    std::unique_lock<std::mutex> lck(lock);
#ifndef _MSC_VER
    pollfd fd;
    fd.fd = socket;
    fd.revents = 0;
#else
	fd_set fdSet;
	struct timeval timeVal;
	memset(&fdSet, 0, sizeof(fd_set));
	FD_SET(socket, &fdSet);
	timeVal.tv_usec = timeoutInMilliseconds * 1000;
	if (timeVal.tv_usec > 1000000) {
		timeVal.tv_sec = timeVal.tv_usec / 1000000;
		timeVal.tv_usec %= 1000000;
	}
#endif
    switch (expected) {
        case Protocol::ProtocolState::CLOSED:
            return state == ProtocolState::CLOSED;
        case Protocol::ProtocolState::OPEN:
            return state == ProtocolState::OPEN;
        case Protocol::ProtocolState::READ_READY:
#ifndef _MSC_VER
            fd.events = POLLRDNORM;
            if (poll(&fd, 1, timeoutInMilliseconds) > 0) {
                if (fd.revents & POLLRDNORM) {
                    return true;
                }
            }
#else
			if (select(1, &fdSet, nullptr, nullptr, &timeVal) == 1) {
				return true;
			}
#endif
            return false;
        case Protocol::ProtocolState::WRITE_READY:
#ifndef _MSC_VER
            fd.events = POLLWRNORM;
            if (poll(&fd, 1, timeoutInMilliseconds) > 0) {
                if (fd.revents & POLLWRNORM) {
                    return true;
                }
            }
#else
			if (select(1, nullptr, &fdSet, nullptr, &timeVal) == 1) {
				return true;
			}
#endif
            return false;
        default:
            return false;
    }
}

void Protocol::close() {
    if (socket > 0) {
#ifndef _MSC_VER
        ::shutdown(socket, SHUT_RDWR);
        ::close(socket);
#else
		::shutdown(socket, SD_BOTH);
		::closesocket(socket);
#endif
        socket = 0;
        host = Host::ALL_INTERFACES6;
        type = ProtocolType::NONE;
        state = ProtocolState::CLOSED;
    }
}

Protocol::ProtocolType Protocol::getType() {
    std::unique_lock<std::mutex> lck(lock);
    return type;
}

Protocol::ProtocolState Protocol::getState() {
    std::unique_lock<std::mutex> lck(lock);
    return state;
}



