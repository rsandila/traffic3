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
// Tests for protocol_tcp4.cpp class
#include <unistd.h>
#include <memory.h>
#include <thread>
#include "catch.hpp"
#include "hippomocks.h"
#include "protocol_udp4.h"
#include "common.h"

TEST_CASE("IPV4: UDP read test", "[ipv4][protocol]") {
    SECTION("Test read with closed socket") {
        MockRepository mocks;
        ProtocolUDP4 protocol;
        mocks.NeverCallFunc(::recvfrom);
        std::vector<char> data;
        data.resize(1024);
        Host hostState = Host::ALL_INTERFACES;
        REQUIRE_FALSE(protocol.read(data, false, hostState));
        REQUIRE(data.size() == 1024);
    }
    SECTION("Test read with connected socket") {
        MockRepository mocks;
        ProtocolUDP4 protocol;
        mocks.ExpectCallFunc(::bind).Return(0);
        mocks.ExpectCallFunc(::connect).Return(0);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES));
        mocks.ExpectCallFunc(::recvfrom).Return(10);
        std::vector<char> data;
        data.resize(1024);
        Host hostState = Host::ALL_INTERFACES;
        REQUIRE(protocol.read(data, true, hostState));
        REQUIRE(data.size() == 10);
    }
    SECTION("Test read failure mode 1 with connected socket") {
        MockRepository mocks;
        ProtocolUDP4 protocol;
        mocks.ExpectCallFunc(::bind).Return(0);
        mocks.ExpectCallFunc(::connect).Return(0);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES));
        mocks.ExpectCallFunc(::recvfrom).Return(0);
        std::vector<char> data;
        data.resize(1024);
        Host hostState = Host::ALL_INTERFACES;
        REQUIRE_FALSE(protocol.read(data, true, hostState));
        REQUIRE(data.size() == 1024);
    }
    SECTION("Test read failure mode 2 with connected socket") {
        MockRepository mocks;
        ProtocolUDP4 protocol;
        mocks.ExpectCallFunc(::bind).Return(0);
        mocks.ExpectCallFunc(::connect).Return(0);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES));
        mocks.ExpectCallFunc(::recvfrom).Return(-1);
        std::vector<char> data;
        data.resize(1024);
        Host hostState = Host::ALL_INTERFACES;
        REQUIRE_FALSE(protocol.read(data, true, hostState));
        REQUIRE(data.size() == 1024);
    }
    
}

TEST_CASE("IPV4: UDP write test", "[ipv4][protocol]") {
    SECTION("Test write with closed socket") {
        MockRepository mocks;
        ProtocolUDP4 protocol;
        mocks.NeverCallFunc(::sendto);
        std::vector<char> data;
        data.resize(1024);
        Host hostState = Host::ALL_INTERFACES;
        REQUIRE_FALSE(protocol.write(data, hostState));
        REQUIRE(data.size() == 1024);
    }
    SECTION("Test write with connected socket") {
        MockRepository mocks;
        ProtocolUDP4 protocol;
        mocks.ExpectCallFunc(::bind).Return(0);
        mocks.ExpectCallFunc(::connect).Return(0);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES));
        mocks.ExpectCallFunc(::sendto).Return(10);
        std::vector<char> data;
        data.resize(10);
        Host hostState = Host::ALL_INTERFACES;
        REQUIRE(protocol.write(data, hostState));
        REQUIRE(data.size() == 10);
    }
    SECTION("Test write failure mode 1 with connected socket") {
        MockRepository mocks;
        ProtocolUDP4 protocol;
        mocks.ExpectCallFunc(::bind).Return(0);
        mocks.ExpectCallFunc(::connect).Return(0);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES));
        mocks.ExpectCallFunc(::sendto).Return(0);
        std::vector<char> data;
        data.resize(1024);
        Host hostState = Host::ALL_INTERFACES;
        REQUIRE_FALSE(protocol.write(data, hostState));
        REQUIRE(data.size() == 1024);
    }
    SECTION("Test write failure mode 2 with connected socket") {
        MockRepository mocks;
        ProtocolUDP4 protocol;
        mocks.ExpectCallFunc(::bind).Return(0);
        mocks.ExpectCallFunc(::connect).Return(0);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES));
        mocks.ExpectCallFunc(::sendto).Return(-1);
        std::vector<char> data;
        data.resize(1024);
        Host hostState = Host::ALL_INTERFACES;
        REQUIRE_FALSE(protocol.write(data, hostState));
        REQUIRE(data.size() == 1024);
    }
}
