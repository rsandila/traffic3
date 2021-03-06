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
#include "protocol_tcp.h"

class ProtocolTCP6: public ProtocolTCP {
public:
    ProtocolTCP6();
    virtual ~ProtocolTCP6();
    virtual std::unique_ptr<Protocol> waitForNewConnection() override;
protected:
private:
    ProtocolTCP6(int socket, socklen_t len, const struct sockaddr * addr);
    ProtocolTCP6(const ProtocolTCP6 &) = delete;
    ProtocolTCP6 & operator=(const ProtocolTCP6 &) = delete;
    ProtocolTCP6(ProtocolTCP6 && other);
};

