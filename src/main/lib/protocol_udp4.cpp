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
        socket(-1), state(ProtocolState::CLOSED), numConnections(0), didRealListen(false) {
}

ProtocolUDP4::ProtocolUDP4(int newSocket, socklen_t len, const struct sockaddr * addr) : host(len, addr),
        type(ProtocolType::CLIENT), socket(newSocket), state(ProtocolState::OPEN), didRealListen(false) {
}

ProtocolUDP4::ProtocolUDP4(ProtocolUDP4 && other) : host(other.host),
type(other.type), socket(other.socket), state(other.state), didRealListen(other.didRealListen) {
    other.socket = -1;
    other.host = Host::ALL_INTERFACES;
    other.type = ProtocolType::NONE;
    other.state = ProtocolState::CLOSED;
}

ProtocolUDP4::~ProtocolUDP4() {
    close();
}

bool ProtocolUDP4::read(std::vector<char> & data, bool allowPartialRead, Host & hostState) {
    std::unique_lock<std::mutex> lck(lock);
    if (state == ProtocolState::CLOSED) {
        return false;
    }
    struct sockaddr addr;
    socklen_t addr_len = sizeof(addr);
    ssize_t numRead = ::recvfrom(socket, &data[0], data.size(), (allowPartialRead)?0:MSG_WAITALL, &addr, &addr_len);
    LOG(DEBUG) << std::this_thread::get_id() << " read " << numRead << std::endl;
    if (numRead < 0) {
        return false;
    }
    if (numRead > 0) {
        data.resize(numRead);
    }
    hostState = Host(addr_len, &addr);
    if (allowPartialRead) {
        return numRead > 0;
    } else {
        return (unsigned long)numRead == data.size();
    }
}

bool ProtocolUDP4::write(const std::vector<char> & data, const Host & hostState) {
    std::unique_lock<std::mutex> lck(lock);
    if (state == ProtocolState::CLOSED || data.size() == 0) {
        return false;
    }
    unsigned long numWritten;
    if (!didRealListen) {
        numWritten = ::send(socket, &data[0], data.size(), 0);
    } else {
        numWritten =::sendto(socket, &data[0], data.size(), 0, hostState.getSockAddress(),
                             hostState.getSockAddressLen());
    }
    LOG(DEBUG) << std::this_thread::get_id() << " wrote " << numWritten << std::endl;
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
    UNUSED(backlog);
    this->host = localHost;
    type = ProtocolType::SERVER;
    state = ProtocolState::OPEN;
    return true;
}

bool ProtocolUDP4::realListen(const Host & localHost) {
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
        type = ProtocolType::CLIENT;
        state = ProtocolState::OPEN;
        didRealListen = true;
        return true;
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
    int optval = 1;
    struct sockaddr addr;
    socklen_t addr_len = sizeof(addr);
    memset(&addr, 0, addr_len);
    setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    if (::bind(socket, &addr, addr_len) == 0) {
        if (::connect(socket, host.getSockAddress(), host.getSockAddressLen()) < 0) {
            ::close(socket);
            return false;
        }
        type = ProtocolType::CLIENT;
        state = ProtocolState::OPEN;
        return true;
    }
    return false;
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
    if (numConnections == 0) {
        numConnections++;
        std::unique_ptr<ProtocolUDP4> returnValue(new ProtocolUDP4());
        if (returnValue->realListen(host)) {
            return std::move(returnValue);
        } else {
            return std::unique_ptr<Protocol>(nullptr);
        }
    }
    while (state != ProtocolState::CLOSED) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return std::unique_ptr<Protocol>(nullptr);
}

