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
        NONE
    };
    virtual ~Protocol() {;};
    virtual bool read(std::vector<char> & data, bool allowPartialRead) { UNUSED(data); UNUSED(allowPartialRead); return false; };
    virtual bool write(const std::vector<char> & data) { UNUSED(data); return false; };
    virtual ProtocolState getState() { return ProtocolState::CLOSED; };
    virtual bool isReady(const ProtocolState & expected, int timeoutInMilliseconds) { UNUSED(expected); UNUSED(timeoutInMilliseconds); return false; };
    virtual bool listen(const Host & host, const int backlog) { UNUSED(host); UNUSED(backlog); return false; };
    virtual bool connect(const Host & host) { UNUSED(host); return false; };
    virtual void close() {;};
    virtual bool isServer() { return getType() == ProtocolType::SERVER; };
    virtual bool isClient() { return getType() == ProtocolType::CLIENT; };
    virtual ProtocolType getType() { return ProtocolType::NONE; };
    virtual std::unique_ptr<Protocol> waitForNewConnection() { return std::unique_ptr<Protocol>(new Protocol()); };
protected:
private:
};
