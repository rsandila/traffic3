#include "server.h"
#include "listener.h"

Server::Server(Protocol & protocol, ContentManagerFactory & contentManagerFactory) : protocolFactory(protocol),
        contentFactory(contentManagerFactory) {
}

Server::~Server() {
    for (auto it = listeners.begin(); it != listeners.end(); ++it) {
        it->Stop();
    }
}

bool Server::addPort(Host & host) {
    std::unique_lock<std::mutex> lck(lock);
    // don't add a second port that is the same
    for (auto it = listeners.cbegin(); it != listeners.cend(); ++it) {
        if (*it == host) {
            return false;
        }
    }
     listeners.push_back(Listener(host, protocolFactory, contentFactory));
    return true;
}


bool Server::stopPort(Host & host) {
    std::unique_lock<std::mutex> lck(lock);
    for (auto it = listeners.begin(); it != listeners.end(); ++it) {
        if (*it == host) {
            return it->Stop();
        }
    }
    return false;
}

const std::vector<Host> Server::getPorts() const noexcept {
    std::unique_lock<std::mutex> lck(lock);
    std::vector<Host> retVal;
    for (auto it = listeners.cbegin(); it != listeners.cend(); ++it) {
        retVal.push_back(it->getHost());
    }
    return std::move(retVal);
}
