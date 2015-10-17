#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include "listener.h"
#include "protocol.h"
#include "contentmanagerfactory.h"

class Server {
public:
    Server(Protocol & protocol, ContentManagerFactory & contentManagerFactory);
    virtual ~Server();
    bool addPort(Host & host);
    bool stopPort(Host & host);
    const std::vector<Host> getPorts() const noexcept;
protected:
private:
    std::vector<Listener> listeners;
    Protocol & protocolFactory;
    ContentManagerFactory & contentFactory;
    mutable std::mutex lock;
};