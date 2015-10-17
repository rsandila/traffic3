#pragma once

#include <sys/socket.h>
#include <poll.h>
#include <netinet/in.h>
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

