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
// Tests for protocol_tcp6.cpp class
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <memory.h>
#include <thread>
#include "catch.hpp"
#include "hippomocks.h"
#include "protocol/protocol_tcp6.h"
#include "lib/common.h"

TEST_CASE("IPV6: TCP read test", "[ipv6][protocol]") {
    SECTION("Test read with closed socket") {
        MockRepository mocks;
        ProtocolTCP6 protocol;
        mocks.NeverCallFunc(::recv);
        std::vector<char> data;
        data.resize(1024);
        Host hostState = Host::ALL_INTERFACES6;
        REQUIRE_FALSE(protocol.read(data, false, hostState));
        REQUIRE(data.size() == 1024);
    }
    SECTION("Test read with connected socket") {
        MockRepository mocks;
        ProtocolTCP6 protocol;
        mocks.ExpectCallFunc(::connect).Return(0);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES6));
		mocks.ExpectCallFunc(::recv).Return(10);
        std::vector<char> data;
        data.resize(1024);
        Host hostState = Host::ALL_INTERFACES6;
        REQUIRE(protocol.read(data, true, hostState));
        REQUIRE(data.size() == 10);
    }
    SECTION("Test read failure mode 1 with connected socket") {
        MockRepository mocks;
        ProtocolTCP6 protocol;
        mocks.ExpectCallFunc(::connect).Return(0);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES6));
		mocks.ExpectCallFunc(::recv).Return(0);
		std::vector<char> data;
        data.resize(1024);
        Host hostState = Host::ALL_INTERFACES6;
        REQUIRE_FALSE(protocol.read(data, true, hostState));
        REQUIRE(data.size() == 1024);
    }
    SECTION("Test read failure mode 2 with connected socket") {
        MockRepository mocks;
        ProtocolTCP6 protocol;
        mocks.ExpectCallFunc(::connect).Return(0);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES6));
		mocks.ExpectCallFunc(::recv).Return(-1);
        std::vector<char> data;
        data.resize(1024);
        Host hostState = Host::ALL_INTERFACES6;
        REQUIRE_FALSE(protocol.read(data, true, hostState));
        REQUIRE(data.size() == 1024);
    }

}

TEST_CASE("IPV6: TCP write test", "[ipv6][protocol]") {
    SECTION("Test write with closed socket") {
        MockRepository mocks;
        ProtocolTCP6 protocol;
		mocks.NeverCallFunc(::send);
        std::vector<char> data;
        data.resize(1024);
        Host hostState = Host::ALL_INTERFACES6;
        REQUIRE_FALSE(protocol.write(data, hostState));
        REQUIRE(data.size() == 1024);
    }
    SECTION("Test write with connected socket") {
        MockRepository mocks;
        ProtocolTCP6 protocol;
        mocks.ExpectCallFunc(::connect).Return(0);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES6));
		mocks.ExpectCallFunc(::send).Return(10);
        std::vector<char> data;
        data.resize(10);
        Host hostState = Host::ALL_INTERFACES6;
        REQUIRE(protocol.write(data, hostState));
        REQUIRE(data.size() == 10);
    }
    SECTION("Test write failure mode 1 with connected socket") {
        MockRepository mocks;
        ProtocolTCP6 protocol;
        mocks.ExpectCallFunc(::connect).Return(0);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES6));
		mocks.ExpectCallFunc(::send).Return(0);
        std::vector<char> data;
        data.resize(1024);
        Host hostState = Host::ALL_INTERFACES6;
        REQUIRE_FALSE(protocol.write(data, hostState));
        REQUIRE(data.size() == 1024);
    }
    SECTION("Test write failure mode 2 with connected socket") {
        MockRepository mocks;
        ProtocolTCP6 protocol;
        mocks.ExpectCallFunc(::connect).Return(0);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES6));
		mocks.ExpectCallFunc(::send).Return(-1);
        std::vector<char> data;
        data.resize(1024);
        Host hostState = Host::ALL_INTERFACES6;
        REQUIRE_FALSE(protocol.write(data, hostState));
        REQUIRE(data.size() == 1024);
    }

}

TEST_CASE("IPV6: TCP isReady test", "[ipv6][protocol]") {
    SECTION("Closed is closed") {
        ProtocolTCP6 protocol;
        REQUIRE(protocol.isReady(Protocol::ProtocolState::CLOSED, 0));
    }
    SECTION("Open is open") {
        MockRepository mocks;
        ProtocolTCP6 protocol;
        mocks.ExpectCallFunc(::connect).Return(0);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES6));
        REQUIRE_FALSE(protocol.isReady(Protocol::ProtocolState::CLOSED, 0));
        REQUIRE(protocol.isReady(Protocol::ProtocolState::OPEN, 0));
    }
    SECTION("Ready for read") {
        MockRepository mocks;
        ProtocolTCP6 protocol;
        mocks.ExpectCallFunc(::connect).Return(0);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES6));
        REQUIRE_FALSE(protocol.isReady(Protocol::ProtocolState::CLOSED, 0));
        REQUIRE(protocol.isReady(Protocol::ProtocolState::OPEN, 0));
#ifndef _MSC_VER
        mocks.ExpectCallFunc(::poll).Do([](struct pollfd fds[], nfds_t nfds, int timeout) -> int { UNUSED(nfds); UNUSED(timeout); fds[0].revents |= POLLRDNORM; return 1; });
        mocks.ExpectCallFunc(::poll).Return(0);
#else
		mocks.ExpectCallFunc(::select).Return(1);
		mocks.ExpectCallFunc(::select).Return(0);
#endif
        REQUIRE(protocol.isReady(Protocol::ProtocolState::READ_READY, 0));
        REQUIRE_FALSE(protocol.isReady(Protocol::ProtocolState::READ_READY, 0));
    }
    SECTION("Ready for write") {
        MockRepository mocks;
        ProtocolTCP6 protocol;
        mocks.ExpectCallFunc(::connect).Return(0);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES6));
        REQUIRE_FALSE(protocol.isReady(Protocol::ProtocolState::CLOSED, 0));
        REQUIRE(protocol.isReady(Protocol::ProtocolState::OPEN, 0));
#ifndef _MSC_VER
		mocks.ExpectCallFunc(::poll).Do([](struct pollfd fds[], nfds_t nfds, int timeout) -> int { UNUSED(nfds); UNUSED(timeout); fds[0].revents |= POLLWRNORM; return 1; });
        mocks.ExpectCallFunc(::poll).Return(0);
#else
		mocks.ExpectCallFunc(::select).Return(1);
		mocks.ExpectCallFunc(::select).Return(0);
#endif
        REQUIRE(protocol.isReady(Protocol::ProtocolState::WRITE_READY, 0));
        REQUIRE_FALSE(protocol.isReady(Protocol::ProtocolState::WRITE_READY, 0));
    }
}

TEST_CASE("IPV6: TCP listen", "[ipv6][protocol]") {
    SECTION("Listen if already connected") {
        MockRepository mocks;
        ProtocolTCP6 protocol;
        mocks.ExpectCallFunc(::connect).Return(0);
        mocks.NeverCallFunc(::bind);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES6));
        REQUIRE_FALSE(protocol.listen(Host::ALL_INTERFACES6, 10));
    }
    SECTION("Listen works") {
        MockRepository mocks;
        ProtocolTCP6 protocol;
        mocks.ExpectCallFunc(::bind).Return(0);
        mocks.ExpectCallFunc(::listen).Return(0);
        REQUIRE(protocol.listen(Host::ALL_INTERFACES6, 10));
        REQUIRE(protocol.getType() == Protocol::ProtocolInstanceType::SERVER);
        REQUIRE(protocol.getState() == Protocol::ProtocolState::OPEN);
    }
    SECTION("Bind fails") {
        MockRepository mocks;
        ProtocolTCP6 protocol;
        mocks.ExpectCallFunc(::bind).Return(-1);
        REQUIRE_FALSE(protocol.listen(Host::ALL_INTERFACES6, 10));
        REQUIRE(protocol.getType() == Protocol::ProtocolInstanceType::NONE);
        REQUIRE(protocol.getState() == Protocol::ProtocolState::CLOSED);
    }
    SECTION("Bind success, ::listen fails") {
        MockRepository mocks;
        ProtocolTCP6 protocol;
        mocks.ExpectCallFunc(::bind).Return(0);
        mocks.ExpectCallFunc(::listen).Return(-1);
        REQUIRE_FALSE(protocol.listen(Host::ALL_INTERFACES6, 10));
        REQUIRE(protocol.getType() == Protocol::ProtocolInstanceType::NONE);
        REQUIRE(protocol.getState() == Protocol::ProtocolState::CLOSED);
    }

}

// connect
TEST_CASE("IPV6: TCP connect", "[ipv6][protocol]") {
    SECTION("Connect if already listening") {
        MockRepository mocks;
        ProtocolTCP6 protocol;
        mocks.ExpectCallFunc(::bind).Return(0);
		mocks.ExpectCallFunc(::listen).Return(0);
        mocks.NeverCallFunc(::connect);
        REQUIRE(protocol.listen(Host::ALL_INTERFACES6, 10));
        REQUIRE_FALSE(protocol.connect(Host::ALL_INTERFACES6));
    }
    SECTION("Connect works") {
        MockRepository mocks;
        ProtocolTCP6 protocol;
        mocks.ExpectCallFunc(::connect).Return(0);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES6));
        REQUIRE(protocol.getType() == Protocol::ProtocolInstanceType::CLIENT);
        REQUIRE(protocol.getState() == Protocol::ProtocolState::OPEN);
    }
    SECTION("Connect fails") {
        MockRepository mocks;
        ProtocolTCP6 protocol;
        mocks.ExpectCallFunc(::connect).Return(-1);
        REQUIRE_FALSE(protocol.connect(Host::ALL_INTERFACES6));
        REQUIRE(protocol.getType() == Protocol::ProtocolInstanceType::NONE);
        REQUIRE(protocol.getState() == Protocol::ProtocolState::CLOSED);
    }
}

// waitfornewconnection
TEST_CASE("IPV6: waitForNewConnection", "[ipv6][protocol]") {
    SECTION("wait if a client") {
        MockRepository mocks;
        ProtocolTCP6 protocol;
        mocks.ExpectCallFunc(::connect).Return(0);
        mocks.NeverCallFunc(::accept);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES6));
        REQUIRE(protocol.getType() == Protocol::ProtocolInstanceType::CLIENT);
        REQUIRE(protocol.getState() == Protocol::ProtocolState::OPEN);
        std::unique_ptr<Protocol> newProtocol = protocol.waitForNewConnection();
        REQUIRE(newProtocol.get() == nullptr);
    }
    SECTION("wait if accept works") {
        MockRepository mocks;
        ProtocolTCP6 protocol;
        mocks.ExpectCallFunc(::socket).Return(1);
        mocks.ExpectCallFunc(::setsockopt).With(_, _, _, _, _).Return(0);
        mocks.ExpectCallFunc(::bind).Return(0);
        mocks.ExpectCallFunc(::listen).Return(0);
        mocks.ExpectCallFunc(::accept).Return(1);
        mocks.ExpectCallFunc(::shutdown).Return(0);
#ifndef _MSC_VER
		mocks.ExpectCallFunc(::close).Return(0);
#else
		mocks.ExpectCallFunc(::closesocket).Return(0);
#endif
        mocks.ExpectCallFunc(::shutdown).Return(0);
#ifndef _MSC_VER
		mocks.ExpectCallFunc(::close).Return(0);
#else
		mocks.ExpectCallFunc(::closesocket).Return(0);
#endif
        REQUIRE(protocol.listen(Host::ALL_INTERFACES6, 10));
        REQUIRE(protocol.getType() == Protocol::ProtocolInstanceType::SERVER);
        REQUIRE(protocol.getState() == Protocol::ProtocolState::OPEN);
        std::unique_ptr<Protocol> newProtocol = protocol.waitForNewConnection();
        REQUIRE(newProtocol.get() != nullptr);
        REQUIRE(newProtocol->getState() == Protocol::ProtocolState::OPEN);
        REQUIRE(newProtocol->getType() == Protocol::ProtocolInstanceType::SERVER_CLIENT);
    }
    SECTION("wait if accept fails") {
        MockRepository mocks;
        ProtocolTCP6 protocol;
        mocks.ExpectCallFunc(::bind).Return(0);
        mocks.ExpectCallFunc(::listen).Return(0);
        mocks.ExpectCallFunc(::accept).Return(-1);
        REQUIRE(protocol.listen(Host::ALL_INTERFACES6, 10));
        REQUIRE(protocol.getType() == Protocol::ProtocolInstanceType::SERVER);
        REQUIRE(protocol.getState() == Protocol::ProtocolState::OPEN);
        std::unique_ptr<Protocol> newProtocol = protocol.waitForNewConnection();
        REQUIRE(newProtocol.get() == nullptr);
    }
}

// close
TEST_CASE("IPV6: close", "[ipv6][protocol]") {
    SECTION("close works") {
        MockRepository mocks;
        ProtocolTCP6 protocol;
       // mocks.ExpectCallFunc(::socket).Return(10);
        mocks.ExpectCallFunc(::connect).Return(0);
        // mocks.ExpectCallFunc(::shutdown); // this causes a crash for some reason
        // mocks.ExpectCallFunc(::close);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES6));
        REQUIRE(protocol.getType() == Protocol::ProtocolInstanceType::CLIENT);
        REQUIRE(protocol.getState() == Protocol::ProtocolState::OPEN);
        protocol.close();
        REQUIRE(protocol.getType() == Protocol::ProtocolInstanceType::NONE);
        REQUIRE(protocol.getState() == Protocol::ProtocolState::CLOSED);
    }
}

TEST_CASE("IPV6: real sending, receiving of data", "[ipv6][protocol]") {
    SECTION("send/receive") {
        bool serverSuccess = false;
        bool testDone = false;
        bool didTimeout = false;
        std::vector<char> testBuffer;
        testBuffer.resize(10);
        ProtocolTCP6 serverProtocol;
        Host listenHost("0:0:0:0:0:0:0:0", 10001, Host::ProtocolPreference::IPV6);
        REQUIRE(serverProtocol.listen(listenHost, 10));
        memcpy(&testBuffer[0], "0123456789", 10);
        std::thread serverThread([&serverProtocol, &testBuffer, &serverSuccess]() -> void {
            std::unique_ptr<Protocol> newProtocol = serverProtocol.waitForNewConnection();
            if (newProtocol.get() != nullptr && newProtocol->isClient() && newProtocol->getState() == Protocol::ProtocolState::OPEN) {
                Host hostState = Host::ALL_INTERFACES6;
                if (newProtocol->write(testBuffer, hostState)) {
                    serverSuccess = true;
                }
            }
        });
        ProtocolTCP6 protocol;
        Host local("::1", 10001, Host::ProtocolPreference::IPV6);
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
        readBuffer.resize(1024);
        Host hostState = Host::ALL_INTERFACES6;
        REQUIRE(protocol.read(readBuffer, true, hostState));
        REQUIRE(readBuffer.size() == testBuffer.size());
        REQUIRE(readBuffer == testBuffer);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        REQUIRE(serverSuccess);
        
        REQUIRE(protocol.getHost() == local);
        REQUIRE(protocol.getBytesRead() == 10);
        REQUIRE(protocol.getBytesWritten() == 0);
        
        nlohmann::json json = protocol.toJson();
        REQUIRE(json.size() == 7);
        REQUIRE(json["host"].is_object());
        REQUIRE(json["type"].get<std::string>() == "Client");
        REQUIRE(json["socket"].is_number_integer());
        REQUIRE(json["numWritten"].get<unsigned>() == 0);
        REQUIRE(json["numRead"].get<unsigned>() == 10);
        REQUIRE(json["state"].get<std::string>() == "Open");
        REQUIRE(json["name"].get<std::string>() == "TCP6");
        testDone = true;
        serverThread.join();
        timeoutThread.join();
        REQUIRE_FALSE(didTimeout);
    }
}

