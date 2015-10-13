#pragma once

#include <vector>
#include <thread>
#include "protocol.h"
#include "contentmanager.h"

class Server {
public:
    Server(Protocol & protocol, ContentManager & contentManager);
    bool addPort(const Host & host, int port);
    bool stopPort(const Host & host, int port);
    const std::vector<int> & getPorts() const noexcept;
protected:
private:
    std::vector<int> ports;
    std::vector<std::thread> listeners;
    Protocol & protocolFactory;
    ContentManager & contentFactory;
};