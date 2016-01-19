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
// Testing for host.h/host.cpp
#ifndef _MSC_VER
#include <sys/socket.h>
#include <poll.h>
#include <netinet/in.h>
#else
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <WS2tcpip.h>
#endif
#include <memory.h>
#include "catch.hpp"
#include "lib/host.h"

TEST_CASE("Validate IPV4 host constructors", "[host][ipv4]") {
    SECTION("IPV4: Test name based constructor") {
        Host googleDNS("google-public-dns-a.google.com", 80, Host::ProtocolPreference::IPV4);
        const struct sockaddr_in * addr = (const struct sockaddr_in *)googleDNS.getSockAddress4();
        REQUIRE(addr != nullptr);
        REQUIRE(googleDNS.getSockAddressLen4() == sizeof(*addr));
        char expectedAddress[] = { 8, 8, 8, 8 };
        REQUIRE(memcmp(&addr->sin_addr, expectedAddress, 4) == 0);
        
        REQUIRE(googleDNS.getProtocolPreference() == Host::ProtocolPreference::IPV4);
        REQUIRE(googleDNS.getPreferredSocketDomain() == AF_INET);
        REQUIRE(googleDNS.getPreferedSockAddressLen() == googleDNS.getSockAddressLen4());
        REQUIRE(googleDNS.getPreferredSockAddress() == googleDNS.getSockAddress4());
    }
    SECTION("IPV4: Test copy constructor") {
        Host googleDNS("google-public-dns-a.google.com", 80, Host::ProtocolPreference::IPV4);
        Host copy(googleDNS);
        const struct sockaddr_in * addr = (const struct sockaddr_in *)copy.getSockAddress4();
        REQUIRE(addr != nullptr);
        REQUIRE(copy.getSockAddressLen4() == sizeof(*addr));
        char expectedAddress[] = { 8, 8, 8, 8 };
        REQUIRE(memcmp(&addr->sin_addr, expectedAddress, 4) == 0);
        
        REQUIRE(googleDNS.getProtocolPreference() == Host::ProtocolPreference::IPV4);
        REQUIRE(googleDNS.getPreferredSocketDomain() == AF_INET);
        REQUIRE(googleDNS.getPreferedSockAddressLen() == googleDNS.getSockAddressLen4());
        REQUIRE(googleDNS.getPreferredSockAddress() == googleDNS.getSockAddress4());
    }
    SECTION("IPV4: Test sock_addr constructor") {
        Host googleDNS("google-public-dns-a.google.com", 80, Host::ProtocolPreference::IPV4);
        Host copy(googleDNS.getSockAddressLen4(), googleDNS.getSockAddress4());
        const struct sockaddr_in * addr = (const struct sockaddr_in *)copy.getSockAddress4();
        REQUIRE(addr != nullptr);
        REQUIRE(copy.getSockAddressLen4() == sizeof(*addr));
        char expectedAddress[] = { 8, 8, 8, 8 };
        REQUIRE(memcmp(&addr->sin_addr, expectedAddress, 4) == 0);
        
        REQUIRE(googleDNS.getProtocolPreference() == Host::ProtocolPreference::IPV4);
        REQUIRE(googleDNS.getPreferredSocketDomain() == AF_INET);
        REQUIRE(googleDNS.getPreferedSockAddressLen() == googleDNS.getSockAddressLen4());
        REQUIRE(googleDNS.getPreferredSockAddress() == googleDNS.getSockAddress4());
    }
    SECTION("IPV4: Test == operator") {
        Host googleDNS1("google-public-dns-a.google.com", 80, Host::ProtocolPreference::IPV4);
        Host googleDNS2("google-public-dns-a.google.com", 80, Host::ProtocolPreference::IPV4);
        Host googleDNS3("google-public-dns-a.google.com", 443, Host::ProtocolPreference::IPV4);
        Host localHost("localhost", 80, Host::ProtocolPreference::IPV4);

        REQUIRE(googleDNS1 == googleDNS2);
        REQUIRE_FALSE(googleDNS1 == googleDNS3);
        REQUIRE_FALSE(googleDNS1 == localHost);
        
        REQUIRE(googleDNS1.getProtocolPreference() == Host::ProtocolPreference::IPV4);
        REQUIRE(googleDNS1.getPreferredSocketDomain() == AF_INET);
        REQUIRE(googleDNS1.getPreferedSockAddressLen() == googleDNS1.getSockAddressLen4());
        REQUIRE(googleDNS1.getPreferredSockAddress() == googleDNS1.getSockAddress4());
    }
}

TEST_CASE("Validate IPV6 host constructors", "[host][ipv6]") {
    SECTION("IPV6: Test name based constructor") {
        Host googleDNS("google-public-dns-a.google.com", 80, Host::ProtocolPreference::IPV6);
        const struct sockaddr_in6 * addr = (const struct sockaddr_in6 *)googleDNS.getSockAddress6();
        REQUIRE(addr != nullptr);
        REQUIRE(googleDNS.getSockAddressLen6() == sizeof(*addr));
        uint8_t expectedAddress[] = { 0x20, 0x01, 0x48, 0x60, 0x48, 0x60, 0, 0, 0, 0, 0, 0, 0, 0, 0x88, 0x88 };
        REQUIRE(memcmp(&addr->sin6_addr, expectedAddress, 16) == 0);
        
        REQUIRE(googleDNS.getProtocolPreference() == Host::ProtocolPreference::IPV6);
        REQUIRE(googleDNS.getPreferredSocketDomain() == AF_INET6);
        REQUIRE(googleDNS.getPreferedSockAddressLen() == googleDNS.getSockAddressLen6());
        REQUIRE(googleDNS.getPreferredSockAddress() == googleDNS.getSockAddress6());
    }
    SECTION("IPV6: Test copy constructor") {
        Host googleDNS("google-public-dns-a.google.com", 80, Host::ProtocolPreference::IPV6);
        Host copy(googleDNS);
        const struct sockaddr_in6 * addr = (const struct sockaddr_in6 *)copy.getSockAddress6();
        REQUIRE(addr != nullptr);
        REQUIRE(copy.getSockAddressLen6() == sizeof(*addr));
        uint8_t expectedAddress[] = { 0x20, 0x01, 0x48, 0x60, 0x48, 0x60, 0, 0, 0, 0, 0, 0, 0, 0, 0x88, 0x88 };
        REQUIRE(memcmp(&addr->sin6_addr, expectedAddress, 16) == 0);
        
        REQUIRE(googleDNS.getProtocolPreference() == Host::ProtocolPreference::IPV6);
        REQUIRE(googleDNS.getPreferredSocketDomain() == AF_INET6);
        REQUIRE(googleDNS.getPreferedSockAddressLen() == googleDNS.getSockAddressLen6());
        REQUIRE(googleDNS.getPreferredSockAddress() == googleDNS.getSockAddress6());
    }
    SECTION("IPV6: Test sock_addr constructor") {
        Host googleDNS("google-public-dns-a.google.com", 80, Host::ProtocolPreference::IPV6);
        Host copy(googleDNS.getSockAddressLen6(), googleDNS.getSockAddress6(), false);
        const struct sockaddr_in6 * addr = (const struct sockaddr_in6 *)copy.getSockAddress6();
        REQUIRE(addr != nullptr);
        REQUIRE(copy.getSockAddressLen6() == sizeof(*addr));
        uint8_t expectedAddress[] = { 0x20, 0x01, 0x48, 0x60, 0x48, 0x60, 0, 0, 0, 0, 0, 0, 0, 0, 0x88, 0x88 };
        REQUIRE(memcmp(&addr->sin6_addr, expectedAddress, 16) == 0);
        
        REQUIRE(googleDNS.getProtocolPreference() == Host::ProtocolPreference::IPV6);
        REQUIRE(googleDNS.getPreferredSocketDomain() == AF_INET6);
        REQUIRE(googleDNS.getPreferedSockAddressLen() == googleDNS.getSockAddressLen6());
        REQUIRE(googleDNS.getPreferredSockAddress() == googleDNS.getSockAddress6());
    }
}

TEST_CASE("Validate protocol type to preference conversion", "[host]") {
    SECTION("IPV4") {
        REQUIRE(Host::ProtocolPreference::IPV4 == convertFromProtocolTypeToPreference(ProtocolType::TCP4));
        REQUIRE(Host::ProtocolPreference::IPV4 == convertFromProtocolTypeToPreference(ProtocolType::UDP4));
    }
    SECTION("IPV6") {
        REQUIRE(Host::ProtocolPreference::IPV6 == convertFromProtocolTypeToPreference(ProtocolType::TCP6));
        REQUIRE(Host::ProtocolPreference::IPV6 == convertFromProtocolTypeToPreference(ProtocolType::UDP6));
    }
}

TEST_CASE("Host toJson", "[host]") {
    SECTION("IPV4") {
        Host googleDNS("google-public-dns-a.google.com", 80, Host::ProtocolPreference::IPV4);
        
        nlohmann::json json = googleDNS.toJson();
        // appveyor returns 7 because IPV6 is disabled
        REQUIRE(json.size() >= 7);
        REQUIRE(json.size() <= 9);
        
        REQUIRE(json["hostName"].get<std::string>() == "google-public-dns-a.google.com");
        REQUIRE(json["port"].get<unsigned>() == 80);
        REQUIRE(json["hasIPv4"].get<bool>() == true);
        if (json.size() == 9) {
            REQUIRE(json["hasIPv6"].get<bool>() == true);
        } else {
            REQUIRE(json["hasIPv6"].get<bool>() == false);
        }
        REQUIRE(json["protocolPreference"].get<std::string>() == "IPv4");
        
        REQUIRE(json["ipv4Name"].get<std::string>() == "google-public-dns-a.google.com");
        REQUIRE(json["ipv4Service"].get<std::string>() == "http");
        if (json.size() == 9) {
            REQUIRE(json["ipv6Name"].get<std::string>() == "google-public-dns-a.google.com");
            REQUIRE(json["ipv6Service"].get<std::string>() == "http");
        }
    }
}
