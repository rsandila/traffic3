#pragma once

#include <sys/socket.h>
#include <poll.h>
#include <netinet/in.h>
#include <string>

class Host {
public:
    Host(const std::string & name, unsigned int port);
    Host(const Host & other);
    Host(socklen_t len, struct sockaddr_in addr);
    const struct sockaddr * getSockAddress() const;
    int getSockAddressLen() const noexcept;
    // TODO
    static Host ALL_INTERFACES;
    static const unsigned DEFAULT_PORT;
    bool operator==(const Host & other) const;
protected:
    bool populateToAddr(const std::string & name, unsigned _port);
private:
    std::string hostName;
    unsigned port;
    struct sockaddr_in addr;
    bool hasAddr;

    friend std::ostream & operator<<(std::ostream & outp, const Host & host);
};

