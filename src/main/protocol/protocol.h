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
#include "lib/host.h"
#include "lib/common.h"
#include "json.hpp"

#ifndef _MSC_VER
typedef int optval_t;
#else
typedef char optval_t;
#endif

class Protocol {
public:
    enum class ProtocolState {
        READ_READY,
        WRITE_READY,
        OPEN,
        CLOSED
    };
    enum class ProtocolInstanceType {
        SERVER,
        CLIENT,
        SERVER_CLIENT,
        NONE
    };
    Protocol(const std::string & protocolName) : host(Host::ALL_INTERFACES6), type(ProtocolInstanceType::NONE),
        socket(-1), state(ProtocolState::CLOSED), name(protocolName), totalWritten(0LL), totalRead(0LL) {;}
    Protocol(Host otherHost, ProtocolInstanceType otherType, int otherSocket, ProtocolState otherState, const std::string & otherName) :
        host(otherHost), type(otherType), socket(otherSocket), state(otherState), name(otherName) {;}
    Protocol(int newSocket, socklen_t len, const struct sockaddr * addr, bool isIPV4, const std::string & otherName) : host(len, addr, isIPV4),
        type(ProtocolInstanceType::SERVER_CLIENT), socket(newSocket), state(ProtocolState::OPEN), name(otherName), totalWritten(0LL), totalRead(0LL) {;}
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
    virtual bool isServer() { return getType() == ProtocolInstanceType::SERVER; };
    virtual bool isClient() { return getType() == ProtocolInstanceType::CLIENT || getType() == ProtocolInstanceType::SERVER_CLIENT; };
    virtual ProtocolInstanceType getType();
    virtual std::unique_ptr<Protocol> waitForNewConnection() { return std::unique_ptr<Protocol>(new Protocol("Undefined")); };
    virtual long long getBytesRead() const noexcept { return totalRead; };
    virtual long long getBytesWritten() const noexcept { return totalWritten; };
    virtual const Host & getHost() const noexcept { return host; };
    virtual const std::string getName() const noexcept { return name; };
    
    virtual nlohmann::json toJson() const noexcept;
protected:
    Host host;
    ProtocolInstanceType type;
    std::mutex lock;
    int socket;
    ProtocolState state;
    std::string name;
    
    virtual void updateBytesRead(long long value);
    virtual void updateBytesWritten(long long value);
private:
    long long totalWritten;
    long long totalRead;
	Protocol & operator=(const Protocol & other) = delete;
	Protocol(const Protocol & other) = delete;
};
