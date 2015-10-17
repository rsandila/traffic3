// Testing for host.h/host.cpp
#include <sys/socket.h>
#include <poll.h>
#include <netinet/in.h>
#include "catch.hpp"
#include "host.h"

TEST_CASE("Validate IPV4 host constructors", "[host][ipv4]") {
    SECTION("IPV4: Test name based constructor") {
        Host googleDNS("google-public-dns-a.google.com", 80);
        const struct sockaddr_in * addr = (const struct sockaddr_in *)googleDNS.getSockAddress();
        REQUIRE(addr != nullptr);
        REQUIRE(googleDNS.getSockAddressLen() == sizeof(*addr));
        char expectedAddress[] = { 8, 8, 8, 8 };
        REQUIRE(memcmp(&addr->sin_addr, expectedAddress, 4) == 0);
    }
    SECTION("IPV4: Test copy constructor") {
        Host googleDNS("google-public-dns-a.google.com", 80);
        Host copy(googleDNS);
        const struct sockaddr_in * addr = (const struct sockaddr_in *)copy.getSockAddress();
        REQUIRE(addr != nullptr);
        REQUIRE(copy.getSockAddressLen() == sizeof(*addr));
        char expectedAddress[] = { 8, 8, 8, 8 };
        REQUIRE(memcmp(&addr->sin_addr, expectedAddress, 4) == 0);
    }
    SECTION("IPV4: Test sock_addr constructor") {
        Host googleDNS("google-public-dns-a.google.com", 80);
        Host copy(googleDNS.getSockAddressLen(), googleDNS.getSockAddress());
        const struct sockaddr_in * addr = (const struct sockaddr_in *)copy.getSockAddress();
        REQUIRE(addr != nullptr);
        REQUIRE(copy.getSockAddressLen() == sizeof(*addr));
        char expectedAddress[] = { 8, 8, 8, 8 };
        REQUIRE(memcmp(&addr->sin_addr, expectedAddress, 4) == 0);        
    }
    SECTION("IPV4: Test == operator") {
        Host googleDNS1("google-public-dns-a.google.com", 80);
        Host googleDNS2("google-public-dns-a.google.com", 80);
        Host googleDNS3("google-public-dns-a.google.com", 443);
        Host localHost("localhost", 80);
        
        REQUIRE(googleDNS1 == googleDNS2);
        REQUIRE_FALSE(googleDNS1 == googleDNS3);
        REQUIRE_FALSE(googleDNS1 == localHost);
    }
}

TEST_CASE("Validate IPV6 host constructors", "[host][ipv6]") {
    SECTION("IPV6: Test name based constructor") {
        Host googleDNS("google-public-dns-a.google.com", 80);
        const struct sockaddr_in6 * addr = (const struct sockaddr_in6 *)googleDNS.getSockAddress6();
        REQUIRE(addr != nullptr);
        REQUIRE(googleDNS.getSockAddressLen6() == sizeof(*addr));
        uint8_t expectedAddress[] = { 0x20, 0x01, 0x48, 0x60, 0x48, 0x60, 0, 0, 0, 0, 0, 0, 0, 0, 0x88, 0x88 };
        REQUIRE(memcmp(&addr->sin6_addr, expectedAddress, 16) == 0);
    }
}