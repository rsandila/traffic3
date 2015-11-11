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

#include <mutex>
#include "protocol.h"

class ProtocolTCP6: public Protocol {
public:
    ProtocolTCP6();
    virtual ~ProtocolTCP6();
    virtual bool read(std::vector<char> & data, bool allowPartialRead, Host & hostState) override;
    virtual bool write(const std::vector<char> & data, const Host & hostState) override;
    virtual ProtocolState getState() override;
    virtual bool isReady(const ProtocolState & expected, int timeoutInMilliseconds) override;
    virtual bool listen(const Host & host, const int backlog) override;
    virtual bool connect(const Host & host) override;
    virtual void close() override;
    virtual ProtocolType getType() override;
    virtual std::unique_ptr<Protocol> waitForNewConnection() override;
protected:
    Host host;
    ProtocolType type;
    std::mutex lock;
    int socket;
    ProtocolState state;
private:
    ProtocolTCP6(int socket, socklen_t len, const struct sockaddr * addr);
    ProtocolTCP6(const ProtocolTCP6 &) = delete;
    ProtocolTCP6 & operator=(const ProtocolTCP6 &) = delete;
    ProtocolTCP6(ProtocolTCP6 && other);
};

