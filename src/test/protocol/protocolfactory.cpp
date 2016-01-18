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
#include "catch.hpp"
#include "hippomocks.h"
#include "protocol/protocolfactory.h"

TEST_CASE("Protocol Factory", "[protocol]") {
    SECTION("TCP4") {
        ProtocolFactory factory(ProtocolType::TCP4);
        std::unique_ptr<Protocol> protocol = factory.createProtocol();
        REQUIRE(protocol.get() != nullptr);
        REQUIRE(dynamic_cast<ProtocolTCP4 *>(protocol.get()) != nullptr);
    }
    
    SECTION("UDP4") {
        ProtocolFactory factory(ProtocolType::UDP4);
        std::unique_ptr<Protocol> protocol = factory.createProtocol();
        REQUIRE(protocol.get() != nullptr);
        REQUIRE(dynamic_cast<ProtocolUDP4 *>(protocol.get()) != nullptr);
    }
    
    SECTION("TCP6") {
        ProtocolFactory factory(ProtocolType::TCP6);
        std::unique_ptr<Protocol> protocol = factory.createProtocol();
        REQUIRE(protocol.get() != nullptr);
        REQUIRE(dynamic_cast<ProtocolTCP6 *>(protocol.get()) != nullptr);
    }
    
    SECTION("UDP6") {
        ProtocolFactory factory(ProtocolType::UDP6);
        std::unique_ptr<Protocol> protocol = factory.createProtocol();
        REQUIRE(protocol.get() != nullptr);
        REQUIRE(dynamic_cast<ProtocolUDP6 *>(protocol.get()) != nullptr);
    }
}
