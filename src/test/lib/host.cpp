// Testing for host.h/host.cpp
#include <sys/socket.h>
#include <poll.h>
#include <netinet/in.h>
#include "catch.hpp"
#include "host.h"

TEST_CASE("Validate host constructors", "[host]") {
    SECTION("Test name based constructor") {
        Host googleDNS("google-public-dns-a.google.com", 80);
        const struct sockaddr_in * addr = (const struct sockaddr_in *)googleDNS.getSockAddress();
        REQUIRE(addr != nullptr);
        REQUIRE(googleDNS.getSockAddressLen() == sizeof(*addr));
        char expectedAddress[] = { 8, 8, 8, 8 };
        REQUIRE(memcmp(&addr->sin_addr, expectedAddress, 4) == 0);
    }
}