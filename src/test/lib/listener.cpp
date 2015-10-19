// Tests for Listener class
#include "catch.hpp"
#include "fakeit.hpp"
#include "listener.h"
#include "protocolfactory.h"

using namespace fakeit;
/*
TEST_CASE("Listener test", "[server]") {
}
*/
TEST_CASE("Listener: Unable to listen", "[server]") {
    SECTION("Bad protocolFactory") {
        ProtocolFactory protocolFactory(ProtocolType::None);
        ContentManagerFactory contentManagerFactory(ContentManagerType::None);
        Listener listener(Host::ALL_INTERFACES, protocolFactory, contentManagerFactory);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        REQUIRE(listener.inErrorState());
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    SECTION("Compare to Host") {
        Host googleDNS("google-public-dns-a.google.com", 80);
        ProtocolFactory protocolFactory(ProtocolType::None);
        ContentManagerFactory contentManagerFactory(ContentManagerType::None);
        Listener listener(Host::ALL_INTERFACES, protocolFactory, contentManagerFactory);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        REQUIRE(listener == Host::ALL_INTERFACES);
        REQUIRE(listener.getHost() == Host::ALL_INTERFACES);
        REQUIRE_FALSE(listener == googleDNS);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
