/*
 * Copyright (C) 2016 Robert Sandilands
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
#include "protocol/protocoltype.h"

TEST_CASE("Protocol Type: convertStringToProtocolType", "[protocol]") {
    SECTION("TCP4") {
        REQUIRE(convertStringToProtocolType("tcp4") == ProtocolType::TCP4);
    }
    SECTION("TCP6") {
        REQUIRE(convertStringToProtocolType("tcp6") == ProtocolType::TCP6);
    }
    SECTION("UDP4") {
        REQUIRE(convertStringToProtocolType("udp4") == ProtocolType::UDP4);
    }
    SECTION("UDP6") {
        REQUIRE(convertStringToProtocolType("udp6") == ProtocolType::UDP6);
    }
    SECTION("None") {
        REQUIRE(convertStringToProtocolType("bla") == ProtocolType::None);
    }
}

TEST_CASE("Protocol Type: protocolTypeToString", "[protocol]") {
    SECTION("TCP4") {
        REQUIRE(protocolTypeToString(ProtocolType::TCP4) == "tcp4");
    }
    SECTION("TCP6") {
        REQUIRE(protocolTypeToString(ProtocolType::TCP6) == "tcp6");
    }
    SECTION("UDP4") {
        REQUIRE(protocolTypeToString(ProtocolType::UDP4) == "udp4");
    }
    SECTION("UDP6") {
        REQUIRE(protocolTypeToString(ProtocolType::UDP6) == "udp6");
    }
    SECTION("None") {
        REQUIRE(protocolTypeToString(ProtocolType::None) == "tcp4");
    }
}