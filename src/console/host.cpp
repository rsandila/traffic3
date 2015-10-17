#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include "host.h"

const unsigned Host::DEFAULT_PORT = 0;
Host Host::ALL_INTERFACES(std::string("0.0.0.0"), Host::DEFAULT_PORT);

Host::Host(const std::string & name, unsigned _port) : hostName(name), hasAddr(false), port(_port) {
    populateToAddr(hostName, _port);
}

Host::Host(const Host & other) : hostName(other.hostName), hasAddr(other.hasAddr), port(other.port) {
    populateToAddr(hostName, port);
}

Host::Host(socklen_t len, struct sockaddr_in otherAddr) : hostName(""), hasAddr(true) {
    memcpy(&addr, &otherAddr, sizeof(otherAddr));
}

const struct sockaddr * Host::getSockAddress() const {
    if (!hasAddr) {
        return nullptr;
    }
    return (const struct sockaddr *)&addr;
}

int Host::getSockAddressLen() const noexcept {
    return sizeof(addr.sin_addr);
}

bool Host::populateToAddr(const std::string & name, unsigned _port) {
    addr.sin_family=AF_INET;
    addr.sin_port=0;
    if (inet_pton(AF_INET, name.c_str(), &addr.sin_addr)<0) {
        return false;
    }
    addr.sin_port = HTONL(_port);
    hasAddr = true;
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
    return (memcmp(&addr, &other.addr, sizeof(addr)) == 0) && (port == other.port);
}

std::ostream & operator<<(std::ostream & outp, const Host & host) {
    outp << host.hostName;
    outp << ":" << host.port;
    return outp;
    }
