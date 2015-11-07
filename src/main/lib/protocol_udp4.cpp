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
#include <sys/socket.h>
#include <poll.h>
#include <netdb.h>
#include <thread>
#include <netinet/in.h>
#include <unistd.h>
#include "protocol_udp4.h"
#include "logging.h"

ProtocolUDP4::ProtocolUDP4() : host(Host::ALL_INTERFACES), type(ProtocolType::NONE),
        socket(-1), state(ProtocolState::CLOSED) {
}

ProtocolUDP4::ProtocolUDP4(int newSocket, socklen_t len, const struct sockaddr * addr) : host(len, addr),
type(ProtocolType::CLIENT), socket(newSocket), state(ProtocolState::OPEN) {
}

ProtocolUDP4::ProtocolUDP4(ProtocolUDP4 && other) : host(other.host),
type(other.type), socket(other.socket), state(other.state) {
    other.socket = -1;
    other.host = Host::ALL_INTERFACES;
    other.type = ProtocolType::NONE;
    other.state = ProtocolState::CLOSED;
}

ProtocolUDP4::~ProtocolUDP4() {
    close();
}

bool ProtocolUDP4::read(std::vector<char> & data, bool allowPartialRead) {
    std::unique_lock<std::mutex> lck(lock);
    if (state == ProtocolState::CLOSED) {
        return false;
    }
    /*
     slen=sizeof( addr );
     result=::recvfrom( sock, data, len, rflag, (struct sockaddr *)&addr, &slen );
     */
    if (allowPartialRead) {
        ssize_t numRead = ::read(socket, &data[0], data.size());
        if (numRead > 0) {
            data.resize(numRead);
        }
        return numRead > 0;
    } else {
        unsigned long offset = 0;
        unsigned long numRead;
        do {
            numRead = ::read(socket, &data[offset], data.size() - offset);
            if (numRead > 0) {
                offset += numRead;
            }
            LOG(DEBUG) << std::this_thread::get_id() << " read " << numRead << std::endl;
        } while (numRead > 0 && offset < data.size());
        return offset == data.size();
    }
}

bool ProtocolUDP4::write(const std::vector<char> & data) {
    std::unique_lock<std::mutex> lck(lock);
    if (state == ProtocolState::CLOSED || data.size() == 0) {
        return false;
    }
    unsigned long numWritten =::sendto(socket, &data[0], data.size(), 0, host.getSockAddress(), host.getSockAddressLen());
    return numWritten == data.size();
}

Protocol::ProtocolState ProtocolUDP4::getState() {
    std::unique_lock<std::mutex> lck(lock);
    return state;
}

bool ProtocolUDP4::isReady(const ProtocolState & expected, int timeoutInMilliseconds) {
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

bool ProtocolUDP4::listen(const Host & localHost, const int backlog) {
    std::unique_lock<std::mutex> lck(lock);
    if (state != ProtocolState::CLOSED || type != ProtocolType::NONE) {
        return false;
    }
    this->host = localHost;
    struct protoent *pr = getprotobyname("udp");
    socket = ::socket(PF_INET, SOCK_DGRAM, pr->p_proto);
    if (socket < 0) {
        return false;
    }
    int optval = 1;
    setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    if (::bind(socket, host.getSockAddress(), host.getSockAddressLen()) == 0) {
        if (::listen(socket, backlog) == 0) {
            type = ProtocolType::SERVER;
            state = ProtocolState::OPEN;
            return true;
        }
    }
    return false;
}

bool ProtocolUDP4::connect(const Host & localHost) {
    std::unique_lock<std::mutex> lck(lock);
    if (state != ProtocolState::CLOSED || type != ProtocolType::NONE) {
        return false;
    }
    this->host = localHost;
    struct protoent *pr = getprotobyname("udp");
    socket = ::socket(PF_INET, SOCK_DGRAM, pr->p_proto);
    if (socket < 0) {
        return false;
    }
    if (::connect(socket, host.getSockAddress(), host.getSockAddressLen()) < 0) {
        ::close(socket);
        return false;
    }
    type = ProtocolType::CLIENT;
    state = ProtocolState::OPEN;
    return true;
}

void ProtocolUDP4::close() {
    if (state != ProtocolState::CLOSED) {
        ::shutdown(socket, SHUT_RDWR);
        ::close(socket);
        socket = 0;
        host = Host::ALL_INTERFACES;
        type = ProtocolType::NONE;
        state = ProtocolState::CLOSED;
    }
}

Protocol::ProtocolType ProtocolUDP4::getType() {
    std::unique_lock<std::mutex> lck(lock);
    return type;
}

std::unique_ptr<Protocol> ProtocolUDP4::waitForNewConnection() {
    // TODO
    return std::unique_ptr<Protocol>(nullptr);
}

