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
#pragma once

#include <vector>
#include <memory>
#include <mutex>
#include "host.h"
#include "common.h"

class Protocol {
public:
    enum class ProtocolState {
        READ_READY,
        WRITE_READY,
        OPEN,
        CLOSED
    };
    enum class ProtocolType {
        SERVER,
        CLIENT,
        SERVER_CLIENT,
        NONE
    };
    Protocol() : host(Host::ALL_INTERFACES6), type(ProtocolType::NONE),
        socket(-1), state(ProtocolState::CLOSED) {;}
    Protocol(Host otherHost, ProtocolType otherType, int otherSocket, ProtocolState otherState) :
        host(otherHost), type(otherType), socket(otherSocket), state(otherState) {;}
    Protocol(int newSocket, socklen_t len, const struct sockaddr * addr, bool isIPV4) : host(len, addr, isIPV4),
        type(ProtocolType::SERVER_CLIENT), socket(newSocket), state(ProtocolState::OPEN) {;}
    virtual ~Protocol() {;};
    virtual bool read(std::vector<char> & data, bool allowPartialRead, Host & hostState) { UNUSED(data);
        UNUSED(allowPartialRead); UNUSED(hostState); return false; };
    virtual bool write(const std::vector<char> & data, const Host & hostState) { UNUSED(data); UNUSED(hostState);
        return false; };
    virtual ProtocolState getState();
    virtual bool isReady(const ProtocolState & expected, int timeoutInMilliseconds);
    virtual bool listen(const Host & localHost, const int backlog) { UNUSED(localHost); UNUSED(backlog); return false; };
    virtual bool connect(const Host & localHost) { UNUSED(localHost); return false; };
    virtual void close();
    virtual bool isServer() { return getType() == ProtocolType::SERVER; };
    virtual bool isClient() { return getType() == ProtocolType::CLIENT || getType() == ProtocolType::SERVER_CLIENT; };
    virtual ProtocolType getType();
    virtual std::unique_ptr<Protocol> waitForNewConnection() { return std::unique_ptr<Protocol>(new Protocol()); };
protected:
    Host host;
    ProtocolType type;
    std::mutex lock;
    int socket;
    ProtocolState state;
private:
};
