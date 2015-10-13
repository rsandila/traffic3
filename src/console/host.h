#pragma once

#include <string>

class Host {
public:
    Host(std::string name);
    Host(const Host & other);
    Host(socklen_t len, struct sockaddr_in addr);
    const struct sockaddr * getSockAddress() const;
    int getSockAddressLen() const;
    // TODO
    static Host ALL_INTERFACES;
protected:
private:
};