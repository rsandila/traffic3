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
// Tests for protocol_udp4.cpp class
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <memory.h>
#include <thread>
#include "catch.hpp"
#include "hippomocks.h"
#include "protocol/protocol_udp4.h"
#include "lib/common.h"

TEST_CASE("IPV4: UDP read test", "[ipv4][protocol]") {
    SECTION("Test read with closed socket") {
        MockRepository mocks;
        ProtocolUDP4 protocol;
        mocks.NeverCallFunc(::recvfrom);
        std::vector<char> data;
        data.resize(1024);
        Host hostState = Host::ALL_INTERFACES4;
        REQUIRE_FALSE(protocol.read(data, false, hostState));
        REQUIRE(data.size() == 1024);
    }
    SECTION("Test read with connected socket") {
        MockRepository mocks;
        ProtocolUDP4 protocol;
        mocks.ExpectCallFunc(::bind).Return(0);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES4));
        // mocks.ExpectCallFunc(::recv).Return(10);
        mocks.ExpectCallFunc(::recvfrom).Return(10);
        std::vector<char> data;
        data.resize(1024);
        Host hostState = Host::ALL_INTERFACES4;
        REQUIRE(protocol.read(data, true, hostState));
        REQUIRE(data.size() == 10);
    }
    SECTION("Test read failure mode 1 with connected socket") {
        MockRepository mocks;
        ProtocolUDP4 protocol;
        mocks.ExpectCallFunc(::bind).Return(0);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES4));
        mocks.ExpectCallFunc(::recvfrom).Return(0);
        // mocks.NeverCallFunc(::recvfrom);
        std::vector<char> data;
        data.resize(1024);
        Host hostState = Host::ALL_INTERFACES4;
        REQUIRE_FALSE(protocol.read(data, true, hostState));
        REQUIRE(data.size() == 1024);
    }
    SECTION("Test read failure mode 2 with connected socket") {
        MockRepository mocks;
        ProtocolUDP4 protocol;
        mocks.ExpectCallFunc(::bind).Return(0);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES4));
        mocks.ExpectCallFunc(::recvfrom).Return(-1);
        // mocks.NeverCallFunc(::recvfrom);
        std::vector<char> data;
        data.resize(1024);
        Host hostState = Host::ALL_INTERFACES4;
        REQUIRE_FALSE(protocol.read(data, true, hostState));
        REQUIRE(data.size() == 1024);
    }
    
}

TEST_CASE("IPV4: UDP write test", "[ipv4][protocol]") {
    SECTION("Test write with closed socket") {
        MockRepository mocks;
        ProtocolUDP4 protocol;
        mocks.NeverCallFunc(::sendto);
        mocks.NeverCallFunc(::send);
        std::vector<char> data;
        data.resize(1024);
        Host hostState = Host::ALL_INTERFACES4;
        REQUIRE_FALSE(protocol.write(data, hostState));
        REQUIRE(data.size() == 1024);
    }
    SECTION("Test write with connected socket") {
        MockRepository mocks;
        ProtocolUDP4 protocol;
        mocks.ExpectCallFunc(::bind).Return(0);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES4));
		mocks.ExpectCallFunc(::sendto).Return(10);
        std::vector<char> data;
        data.resize(10);
        Host hostState = Host::ALL_INTERFACES4;
        REQUIRE(protocol.write(data, hostState));
        REQUIRE(data.size() == 10);
    }
    SECTION("Test write failure mode 1 with connected socket") {
        MockRepository mocks;
        ProtocolUDP4 protocol;
        mocks.ExpectCallFunc(::bind).Return(0);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES4));
		mocks.ExpectCallFunc(::sendto).Return(0);
        std::vector<char> data;
        data.resize(1024);
        Host hostState = Host::ALL_INTERFACES4;
        REQUIRE_FALSE(protocol.write(data, hostState));
        REQUIRE(data.size() == 1024);
    }
    SECTION("Test write failure mode 2 with connected socket") {
        MockRepository mocks;
        ProtocolUDP4 protocol;
        mocks.ExpectCallFunc(::bind).Return(0);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES4));
		mocks.ExpectCallFunc(::sendto).Return(-1);
        std::vector<char> data;
        data.resize(1024);
        Host hostState = Host::ALL_INTERFACES4;
        REQUIRE_FALSE(protocol.write(data, hostState));
        REQUIRE(data.size() == 1024);
    }
}

TEST_CASE("IPV4: real sending, receiving of UDP data", "[ipv4][protocol]") {
    SECTION("send/receive") {
        bool serverSuccess = false;
        bool testDone = false;
        bool didTimeout = false;
        std::vector<char> testBuffer, testBuffer2;
        testBuffer.resize(10);
        testBuffer2.resize(10);
        ProtocolUDP4 serverProtocol;
        Host listenHost("0.0.0.0", 10001, Host::ProtocolPreference::IPV4);
        REQUIRE(serverProtocol.listen(listenHost, 10));
        memcpy(&testBuffer[0],  "0123456789", 10);
        memcpy(&testBuffer2[0], "1234567890", 10);
        std::thread serverThread([&serverProtocol, &testBuffer, &serverSuccess, &testBuffer2]() -> void {
            std::unique_ptr<Protocol> newProtocol = serverProtocol.waitForNewConnection();
            if (newProtocol.get() != nullptr && newProtocol->isClient() && newProtocol->getState() == Protocol::ProtocolState::OPEN) {
                Host hostState = Host::ALL_INTERFACES4;
                std::vector<char> readBuffer;
                readBuffer.resize(1024);
                if (newProtocol->read(readBuffer, false, hostState)) {
                    serverSuccess = (readBuffer == testBuffer2);
                    if (newProtocol->write(testBuffer, hostState)) {
                        serverSuccess &= true;
                    }
                }
            }
        });
        ProtocolUDP4 protocol;
        Host local("127.0.0.1", 10001, Host::ProtocolPreference::IPV4);
        std::thread timeoutThread([&serverProtocol, &protocol, &testDone, &didTimeout]() -> void {
            int retry = 600;
            while (--retry > 0 && !testDone) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            if (!testDone) {
                serverProtocol.close();
                protocol.close();
                didTimeout = true;
            }
        });
        REQUIRE(protocol.connect(local));
        std::vector<char> readBuffer;
        readBuffer.resize(100);
        Host hostState = Host::ALL_INTERFACES4;
        REQUIRE(protocol.write(testBuffer2, hostState));
        REQUIRE(protocol.read(readBuffer, false, hostState));
        REQUIRE(readBuffer.size() == testBuffer.size());
        REQUIRE(readBuffer == testBuffer);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        REQUIRE(serverSuccess);
        
        REQUIRE(protocol.getHost() == local);
        REQUIRE(protocol.getBytesRead() == 10);
        REQUIRE(protocol.getBytesWritten() == 10);
        
        nlohmann::json json = protocol.toJson();
        REQUIRE(json.size() == 7);
        REQUIRE(json["host"].is_object());
        REQUIRE(json["type"].get<std::string>() == "Client");
        REQUIRE(json["socket"].is_number_integer());
        REQUIRE(json["numWritten"].get<unsigned>() == 10);
        REQUIRE(json["numRead"].get<unsigned>() == 10);
        REQUIRE(json["state"].get<std::string>() == "Open");
        REQUIRE(json["name"].get<std::string>() == "UDP4");
        testDone = true;
        serverThread.join();
        timeoutThread.join();
        REQUIRE_FALSE(didTimeout);
    }
}
