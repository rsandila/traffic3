/*
 * Copyright (C) 2003 Robert Sandilands (Pty) Ltd.
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
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <stdexcept>
#include "host.h"

const unsigned Host::DEFAULT_PORT = 0;
Host Host::ALL_INTERFACES(std::string("0.0.0.0"), Host::DEFAULT_PORT);

Host::Host(const std::string & name, unsigned _port) : hostName(name), port(_port), hasAddr(false), hasAddr6(false) {
    populateToAddr(hostName, _port);
    populateToAddr6(name, _port);
}

Host::Host(const Host & other) : hostName(other.hostName), port(other.port), hasAddr(other.hasAddr), hasAddr6(other.hasAddr6) {
    populateToAddr(hostName, port);
    populateToAddr6(hostName, port);
}

Host::Host(socklen_t len, const struct sockaddr * otherAddr, bool isIPV4) : hostName(""),
        hasAddr(isIPV4), hasAddr6(!isIPV4) {
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

const struct sockaddr * Host::getSockAddress() const {
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

socklen_t Host::getSockAddressLen() const noexcept {
    return sizeof(addr);
}

socklen_t Host::getSockAddressLen6() const noexcept {
    return sizeof(addr6);
}

bool Host::populateToAddr(const std::string & name, unsigned _port) {
    memset(&addr, 0, sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_port=0;
    if (inet_pton(addr.sin_family, name.c_str(), &addr.sin_addr) <= 0) {
        struct hostent * h = gethostbyname2(name.c_str(), addr.sin_family);
        if (h) {
            memcpy(&addr.sin_addr, h->h_addr_list[0], sizeof(addr.sin_addr));
        } else {
            return false;
        }
    }
    addr.sin_port = htons(_port);
    hasAddr = true;
    return true;
}

bool Host::populateToAddr6(const std::string & name, unsigned _port) {
    memset(&addr6, 0, sizeof(addr6));
    addr6.sin6_family=AF_INET6;
    addr6.sin6_port=0;
    if (inet_pton(addr6.sin6_family, name.c_str(), &addr6.sin6_addr) <= 0) {
        struct hostent * h = gethostbyname2(name.c_str(), addr6.sin6_family);
        if (h) {
            memcpy(&addr6.sin6_addr, h->h_addr_list[0], sizeof(addr6.sin6_addr));
        } else {
            return false;
        }
    }
    addr6.sin6_port = htons(_port);
    hasAddr6 = true;
    return true;
}

bool Host::operator==(const Host & other) const {
    if (!other.hasAddr) {
        return false; // if not a valid addr then no point in comparing
    }
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
