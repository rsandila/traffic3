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
#include <poll.h>
#include <unistd.h>
#include "protocol.h"

bool Protocol::isReady(const ProtocolState & expected, int timeoutInMilliseconds) {
    std::unique_lock<std::mutex> lck(lock);
    pollfd fd;
    fd.fd = socket;
    fd.revents = 0;
    switch (expected) {
        case Protocol::ProtocolState::CLOSED:
            return state == ProtocolState::CLOSED;
        case Protocol::ProtocolState::OPEN:
            return state == ProtocolState::OPEN;
        case Protocol::ProtocolState::READ_READY:
            fd.events = POLLRDNORM;
            if (poll(&fd, 1, timeoutInMilliseconds) > 0) {
                if (fd.revents & POLLRDNORM) {
                    return true;
                }
            }
            return false;
        case Protocol::ProtocolState::WRITE_READY:
            fd.events = POLLWRNORM;
            if (poll(&fd, 1, timeoutInMilliseconds) > 0) {
                if (fd.revents & POLLWRNORM) {
                    return true;
                }
            }
            return false;
        default:
            return false;
    }
}

void Protocol::close() {
    // std::unique_lock<std::mutex> lck(lock);
    if (state != ProtocolState::CLOSED) {
        ::shutdown(socket, SHUT_RDWR);
        ::close(socket);
        socket = 0;
        host = Host::ALL_INTERFACES;
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



