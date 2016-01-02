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
#ifndef _MSC_VER
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#else
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#endif
#include <iostream>
#include <stdexcept>
#include "host.h"

#ifdef _MSC_VER

class WindowsInit {
public:
	WindowsInit() {
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);
	}
	~WindowsInit() {
		WSACleanup();
	}
};

static WindowsInit globalWindowsInit;
#endif
const unsigned Host::DEFAULT_PORT = 0;
Host Host::ALL_INTERFACES4(std::string("0.0.0.0"), Host::DEFAULT_PORT, Host::ProtocolPreference::IPV4);
Host Host::ALL_INTERFACES6(std::string("::"), Host::DEFAULT_PORT, Host::ProtocolPreference::IPV6);

Host::Host(const std::string & name, unsigned _port, const ProtocolPreference preference) : hostName(name), port(_port), hasAddr(false), hasAddr6(false), protocolPreference(preference) {
    populateToAddr(hostName, _port);
    populateToAddr6(name, _port);
}

Host::Host(const Host & other) : hostName(other.hostName), port(other.port), hasAddr(other.hasAddr), hasAddr6(other.hasAddr6), protocolPreference(other.protocolPreference) {
    if (hasAddr) {
        memcpy(&addr, &other.addr, sizeof(addr));
    } else {
        populateToAddr(hostName, port);
    }
    if (hasAddr6) {
        memcpy(&addr6, &other.addr6, sizeof(addr6));
    } else {
        populateToAddr6(hostName, port);
    }
}

Host::Host(socklen_t len, const struct sockaddr * otherAddr, bool isIPV4) : hostName(""),
        hasAddr(isIPV4), hasAddr6(!isIPV4),
        protocolPreference((isIPV4)?ProtocolPreference::IPV4:ProtocolPreference::IPV6) {
    if (isIPV4) {
        if (len < sizeof(addr)) {
            throw new std::length_error("addr not big enough");
        }
        memcpy(&addr, otherAddr, sizeof(addr));
    } else {
        if (len < sizeof(addr6)) {
            throw new std::length_error("addr not big enough");
        }
        memcpy(&addr6, otherAddr, sizeof(addr6));
    }
}

const struct sockaddr * Host::getPreferredSockAddress() const {
    switch (protocolPreference) {
        case ProtocolPreference::IPV4:
            return getSockAddress4();
        case ProtocolPreference::IPV6:
            return getSockAddress6();
        default:
            if (hasAddr6) {
                return getSockAddress6();
            } else {
                return getSockAddress4();
            }
    }
}

const struct sockaddr * Host::getSockAddress4() const {
    if (!hasAddr) {
        return nullptr;
    }
    return (const struct sockaddr *)&addr;
}

const struct sockaddr * Host::getSockAddress6() const {
    if (!hasAddr6) {
        return nullptr;
    }
    return (const struct sockaddr *)&addr6;
}

socklen_t Host::getPreferedSockAddressLen() const noexcept {
    switch (protocolPreference) {
        case ProtocolPreference::IPV4:
            return getSockAddressLen4();
        case ProtocolPreference::IPV6:
            return getSockAddressLen6();
        default:
            if (hasAddr6) {
                return getSockAddressLen6();
            } else {
                return getSockAddressLen4();
            }
    }
}

socklen_t Host::getSockAddressLen4() const noexcept {
    return sizeof(addr);
}

socklen_t Host::getSockAddressLen6() const noexcept {
    return sizeof(addr6);
}

int Host::getPreferredSocketDomain() const noexcept {
    switch (protocolPreference) {
        case ProtocolPreference::IPV4:
            return AF_INET;
        case ProtocolPreference::IPV6:
            return AF_INET6;
        default:
            if (hasAddr6) {
                return AF_INET6;
            } else {
                return AF_INET;
            }
    }
    
}

Host::ProtocolPreference Host::getProtocolPreference() const noexcept {
    return protocolPreference;
}

bool Host::populateToAddr(const std::string & name, unsigned _port) {
    if (name.empty()) {
        return false;
    }
	struct addrinfo hints;
	struct addrinfo *result;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = 0;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;

	if (getaddrinfo(name.c_str(), nullptr, &hints, &result) == 0) {
        memcpy(&addr, result->ai_addr, sizeof(addr));
		addr.sin_port = htons(_port);
		freeaddrinfo(result);
        hasAddr = true;
		return true;
	}
	return false;
}

bool Host::populateToAddr6(const std::string & name, unsigned _port) {
    if (name.empty()) {
        return false;
    }
	struct addrinfo hints;
	struct addrinfo *result;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = 0;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;

	if (getaddrinfo(name.c_str(), nullptr, &hints, &result) == 0) {
        memcpy(&addr6, result->ai_addr, sizeof(addr6));
		addr6.sin6_port = htons(_port);
		freeaddrinfo(result);
        hasAddr6 = true;
		return true;
    }
	return false;
}

bool Host::operator==(const Host & other) const {
    // if it has a name and it differs then exit
    if (other.hostName.size() > 0 && hostName.size() > 0) {
        if (hostName != other.hostName) {
            return false;
        }
    }
    bool retval = true;
    if (hasAddr) {
        retval &= (memcmp(&addr, &other.addr, sizeof(addr)) == 0) && (port == other.port);
    }
    if (hasAddr6) {
        retval &= (memcmp(&addr6, &other.addr6, sizeof(addr6)) == 0) && (port == other.port);
    }
    return retval;
}

std::ostream & operator<<(std::ostream & outp, const Host & host) {
    outp << host.hostName;
    outp << ":" << host.port;
    return outp;
}

Host::ProtocolPreference convertFromProtocolTypeToPreference(const ProtocolType type) {
    switch (type) {
        case ProtocolType::UDP4:
            // fall through on purpose
        case ProtocolType::TCP4:
            return Host::ProtocolPreference::IPV4;
        case ProtocolType::TCP6:
            // fall through on purpose
        case ProtocolType::UDP6:
            return Host::ProtocolPreference::IPV6;
        case ProtocolType::None:
            return Host::ProtocolPreference::ANY;
    }
}
