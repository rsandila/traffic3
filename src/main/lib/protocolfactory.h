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

#include "protocol.h"
#include "protocol_tcp4.h"
#include "protocol_udp4.h"
#include "protocoltype.h"

class ProtocolFactory {
public:
    ProtocolFactory(const ProtocolType & _type) : type(_type) {
    };
    virtual ~ProtocolFactory() {;};
    virtual std::unique_ptr<Protocol> createProtocol() {
        switch (type) {
            case ProtocolType::TCP4:
                return std::unique_ptr<Protocol>(new ProtocolTCP4());
            case ProtocolType::UDP4:
                return std::unique_ptr<Protocol>(new ProtocolUDP4());
            default:
                return std::unique_ptr<Protocol>(new Protocol());
        }
    };
private:
    ProtocolType type;
};
