// Tests for protocol_tcp4.cpp class
#include "catch.hpp"
#include "hippomocks.h"
#include "protocol_tcp4.h"

TEST_CASE("IPV4: TCP read test", "[ipv4][protocol]") {
    SECTION("Test read with closed socket") {
        MockRepository mocks;
        ProtocolTCP4 protocol;
        mocks.NeverCallFunc(read);
        std::vector<char> data;
        data.resize(1024);
        REQUIRE_FALSE(protocol.read(data));
        REQUIRE(data.size() == 1024);
    }
    SECTION("Test read with connected socket") {
        MockRepository mocks;
        ProtocolTCP4 protocol;
        mocks.ExpectCallFunc(::connect).Return(0);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES));
        mocks.ExpectCallFunc(::read).Return(10);
        std::vector<char> data;
        data.resize(1024);
        REQUIRE(protocol.read(data));
        REQUIRE(data.size() == 10);
    }
    SECTION("Test read failure mode 1 with connected socket") {
        MockRepository mocks;
        ProtocolTCP4 protocol;
        mocks.ExpectCallFunc(::connect).Return(0);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES));
        mocks.ExpectCallFunc(::read).Return(0);
        std::vector<char> data;
        data.resize(1024);
        REQUIRE_FALSE(protocol.read(data));
        REQUIRE(data.size() == 1024);
    }
    SECTION("Test read failure mode 2 with connected socket") {
        MockRepository mocks;
        ProtocolTCP4 protocol;
        mocks.ExpectCallFunc(::connect).Return(0);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES));
        mocks.ExpectCallFunc(::read).Return(-1);
        std::vector<char> data;
        data.resize(1024);
        REQUIRE_FALSE(protocol.read(data));
        REQUIRE(data.size() == 1024);
    }

}

TEST_CASE("IPV4: TCP write test", "[ipv4][protocol]") {
    SECTION("Test write with closed socket") {
        MockRepository mocks;
        ProtocolTCP4 protocol;
        mocks.NeverCallFunc(::write);
        std::vector<char> data;
        data.resize(1024);
        REQUIRE_FALSE(protocol.write(data));
        REQUIRE(data.size() == 1024);
    }
    SECTION("Test write with connected socket") {
        MockRepository mocks;
        ProtocolTCP4 protocol;
        mocks.ExpectCallFunc(::connect).Return(0);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES));
        mocks.ExpectCallFunc(::write).Return(10);
        std::vector<char> data;
        data.resize(10);
        REQUIRE(protocol.write(data));
        REQUIRE(data.size() == 10);
    }
    SECTION("Test write failure mode 1 with connected socket") {
        MockRepository mocks;
        ProtocolTCP4 protocol;
        mocks.ExpectCallFunc(::connect).Return(0);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES));
        mocks.ExpectCallFunc(::write).Return(0);
        std::vector<char> data;
        data.resize(1024);
        REQUIRE_FALSE(protocol.write(data));
        REQUIRE(data.size() == 1024);
    }
    SECTION("Test write failure mode 2 with connected socket") {
        MockRepository mocks;
        ProtocolTCP4 protocol;
        mocks.ExpectCallFunc(::connect).Return(0);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES));
        mocks.ExpectCallFunc(::write).Return(-1);
        std::vector<char> data;
        data.resize(1024);
        REQUIRE_FALSE(protocol.write(data));
        REQUIRE(data.size() == 1024);
    }

}

TEST_CASE("IPV4: TCP isReady test", "[ipv4][protocol]") {
    SECTION("Closed is closed") {
        ProtocolTCP4 protocol;
        REQUIRE(protocol.isReady(Protocol::ProtocolState::CLOSED, 0));
    }
    SECTION("Open is open") {
        MockRepository mocks;
        ProtocolTCP4 protocol;
        mocks.ExpectCallFunc(::connect).Return(0);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES));
        REQUIRE_FALSE(protocol.isReady(Protocol::ProtocolState::CLOSED, 0));
        REQUIRE(protocol.isReady(Protocol::ProtocolState::OPEN, 0));
    }
    SECTION("Ready for read") {
        MockRepository mocks;
        ProtocolTCP4 protocol;
        mocks.ExpectCallFunc(::connect).Return(0);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES));
        REQUIRE_FALSE(protocol.isReady(Protocol::ProtocolState::CLOSED, 0));
        REQUIRE(protocol.isReady(Protocol::ProtocolState::OPEN, 0));
        mocks.ExpectCallFunc(::poll).Do([](struct pollfd fds[], nfds_t nfds, int timeout) -> int { fds[0].revents |= POLLRDNORM; return 1; });
        mocks.ExpectCallFunc(::poll).Return(0);
        REQUIRE(protocol.isReady(Protocol::ProtocolState::READ_READY, 0));
        REQUIRE_FALSE(protocol.isReady(Protocol::ProtocolState::READ_READY, 0));
    }
    SECTION("Ready for write") {
        MockRepository mocks;
        ProtocolTCP4 protocol;
        mocks.ExpectCallFunc(::connect).Return(0);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES));
        REQUIRE_FALSE(protocol.isReady(Protocol::ProtocolState::CLOSED, 0));
        REQUIRE(protocol.isReady(Protocol::ProtocolState::OPEN, 0));
        mocks.ExpectCallFunc(::poll).Do([](struct pollfd fds[], nfds_t nfds, int timeout) -> int { fds[0].revents |= POLLWRNORM; return 1; });
        mocks.ExpectCallFunc(::poll).Return(0);
        REQUIRE(protocol.isReady(Protocol::ProtocolState::WRITE_READY, 0));
        REQUIRE_FALSE(protocol.isReady(Protocol::ProtocolState::WRITE_READY, 0));
    }
}

TEST_CASE("IPV4: TCP listen", "[ipv4][protocol]") {
    SECTION("Listen if already connected") {
        MockRepository mocks;
        ProtocolTCP4 protocol;
        mocks.ExpectCallFunc(::connect).Return(0);
        mocks.NeverCallFunc(::bind);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES));
        REQUIRE_FALSE(protocol.listen(Host::ALL_INTERFACES, 10));
    }
    SECTION("Listen works") {
        MockRepository mocks;
        ProtocolTCP4 protocol;
        mocks.ExpectCallFunc(::bind).Return(0);
        mocks.ExpectCallFunc(::listen).Return(0);
        REQUIRE(protocol.listen(Host::ALL_INTERFACES, 10));
        REQUIRE(protocol.getType() == Protocol::ProtocolType::SERVER);
        REQUIRE(protocol.getState() == Protocol::ProtocolState::OPEN);
    }
    SECTION("Bind fails") {
        MockRepository mocks;
        ProtocolTCP4 protocol;
        mocks.ExpectCallFunc(::bind).Return(-1);
        REQUIRE_FALSE(protocol.listen(Host::ALL_INTERFACES, 10));
        REQUIRE(protocol.getType() == Protocol::ProtocolType::NONE);
        REQUIRE(protocol.getState() == Protocol::ProtocolState::CLOSED);
    }
    SECTION("Bind success, ::listen fails") {
        MockRepository mocks;
        ProtocolTCP4 protocol;
        mocks.ExpectCallFunc(::bind).Return(0);
        mocks.ExpectCallFunc(::listen).Return(-1);
        REQUIRE_FALSE(protocol.listen(Host::ALL_INTERFACES, 10));
        REQUIRE(protocol.getType() == Protocol::ProtocolType::NONE);
        REQUIRE(protocol.getState() == Protocol::ProtocolState::CLOSED);
    }

}

// connect
TEST_CASE("IPV4: TCP connect", "[ipv4][protocol]") {
    SECTION("Connect if already listening") {
        MockRepository mocks;
        ProtocolTCP4 protocol;
        mocks.ExpectCallFunc(::bind).Return(0);
        mocks.NeverCallFunc(::connect);
        REQUIRE(protocol.listen(Host::ALL_INTERFACES, 10));
        REQUIRE_FALSE(protocol.connect(Host::ALL_INTERFACES));
    }
    SECTION("Connect works") {
        MockRepository mocks;
        ProtocolTCP4 protocol;
        mocks.ExpectCallFunc(::connect).Return(0);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES));
        REQUIRE(protocol.getType() == Protocol::ProtocolType::CLIENT);
        REQUIRE(protocol.getState() == Protocol::ProtocolState::OPEN);
    }
    SECTION("Connect fails") {
        MockRepository mocks;
        ProtocolTCP4 protocol;
        mocks.ExpectCallFunc(::connect).Return(-1);
        REQUIRE_FALSE(protocol.connect(Host::ALL_INTERFACES));
        REQUIRE(protocol.getType() == Protocol::ProtocolType::NONE);
        REQUIRE(protocol.getState() == Protocol::ProtocolState::CLOSED);
    }
}

// waitfornewconnection
TEST_CASE("IPV4: waitForNewConnection", "[ipv4][protocol]") {
    SECTION("wait if a client") {
        MockRepository mocks;
        ProtocolTCP4 protocol;
        mocks.ExpectCallFunc(::connect).Return(0);
        mocks.NeverCallFunc(::accept);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES));
        REQUIRE(protocol.getType() == Protocol::ProtocolType::CLIENT);
        REQUIRE(protocol.getState() == Protocol::ProtocolState::OPEN);
        std::unique_ptr<Protocol> newProtocol = protocol.waitForNewConnection();
        REQUIRE(newProtocol.get() == nullptr);
    }
    SECTION("wait if accept works") {
        MockRepository mocks;
        ProtocolTCP4 protocol;
        mocks.ExpectCallFunc(::bind).Return(0);
        mocks.ExpectCallFunc(::listen).Return(0);
        mocks.ExpectCallFunc(::accept).Return(1);
        REQUIRE(protocol.listen(Host::ALL_INTERFACES, 10));
        REQUIRE(protocol.getType() == Protocol::ProtocolType::SERVER);
        REQUIRE(protocol.getState() == Protocol::ProtocolState::OPEN);
        std::unique_ptr<Protocol> newProtocol = protocol.waitForNewConnection();
        REQUIRE(newProtocol.get() != nullptr);
        REQUIRE(newProtocol->getState() == Protocol::ProtocolState::OPEN);
        REQUIRE(newProtocol->getType() == Protocol::ProtocolType::CLIENT);
    }
    SECTION("wait if accept fails") {
        MockRepository mocks;
        ProtocolTCP4 protocol;
        mocks.ExpectCallFunc(::bind).Return(0);
        mocks.ExpectCallFunc(::listen).Return(0);
        mocks.ExpectCallFunc(::accept).Return(-1);
        REQUIRE(protocol.listen(Host::ALL_INTERFACES, 10));
        REQUIRE(protocol.getType() == Protocol::ProtocolType::SERVER);
        REQUIRE(protocol.getState() == Protocol::ProtocolState::OPEN);
        std::unique_ptr<Protocol> newProtocol = protocol.waitForNewConnection();
        REQUIRE(newProtocol.get() == nullptr);
    }
}

// close
TEST_CASE("IPV4: close", "[ipv4][protocol]") {
    SECTION("close works") {
        MockRepository mocks;
        ProtocolTCP4 protocol;
       // mocks.ExpectCallFunc(::socket).Return(10);
        mocks.ExpectCallFunc(::connect).Return(0);
        // mocks.ExpectCallFunc(::shutdown); // this causes a crash for some reason
        // mocks.ExpectCallFunc(::close);
        REQUIRE(protocol.connect(Host::ALL_INTERFACES));
        REQUIRE(protocol.getType() == Protocol::ProtocolType::CLIENT);
        REQUIRE(protocol.getState() == Protocol::ProtocolState::OPEN);
        protocol.close();
        REQUIRE(protocol.getType() == Protocol::ProtocolType::NONE);
        REQUIRE(protocol.getState() == Protocol::ProtocolState::CLOSED);
    }
}