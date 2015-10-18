#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include "listener.h"
#include "protocolfactory.h"
#include "contentmanagerfactory.h"

class Server {
public:
    Server(ProtocolFactory & protocolFactory, ContentManagerFactory & contentManagerFactory);
    virtual ~Server();
    bool addPort(Host & host);
    bool stopPort(Host & host);
    const std::vector<Host> getPorts() const noexcept;
protected:
private:
    std::vector<std::unique_ptr<Listener>> listeners;
    ProtocolFactory & protocolFactory;
    ContentManagerFactory & contentFactory;
    mutable std::mutex lock;
};