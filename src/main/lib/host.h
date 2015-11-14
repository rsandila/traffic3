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
#pragma once

#ifndef _MSC_VER
#include <sys/socket.h>
#include <poll.h>
#include <netinet/in.h>
#else
#include <winsock2.h>
#include <WS2tcpip.h>
#endif
#include <string>

class Host {
public:
    Host(const std::string & name, unsigned int port);
    Host(const Host & other);
    Host(socklen_t len, const struct sockaddr * addr, bool isIPV4 = true);
    const struct sockaddr * getSockAddress() const;
    const struct sockaddr * getSockAddress6() const;
    socklen_t getSockAddressLen() const noexcept;
    socklen_t getSockAddressLen6() const noexcept;
    // TODO
    static Host ALL_INTERFACES;
    static const unsigned DEFAULT_PORT;
    bool operator==(const Host & other) const;
protected:
    bool populateToAddr(const std::string & name, unsigned _port);
    bool populateToAddr6(const std::string & name, unsigned _port);
private:
    std::string hostName;
    unsigned port;
    struct sockaddr_in addr;
    struct sockaddr_in6 addr6;
    bool hasAddr, hasAddr6;

    friend std::ostream & operator<<(std::ostream & outp, const Host & host);
};

