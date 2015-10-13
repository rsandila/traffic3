#include <netinet/in.h>
#include "host.h"

Host Host::ALL_INTERFACES(std::string("0.0.0.0"));

Host::Host(std::string name) {
    // TODO
}

Host::Host(const Host & other) {
    // TODO
}

Host::Host(socklen_t len, struct sockaddr_in addr) {
    // TODO
}

const struct sockaddr * Host::getSockAddress() const {
    // TODO
    return nullptr;
}

int Host::getSockAddressLen() const {
    // TODO
    return 0;
}
