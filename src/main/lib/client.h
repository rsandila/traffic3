#pragma once

#include <map>
#include <vector>
#include "protocolfactory.h"
#include "contentmanagerfactory.h"
#include "host.h"

class Client {
public:
    Client();
    virtual ~Client();
    bool startClients(unsigned clientId, unsigned num_clients, ProtocolFactory & _protocolFactory, ContentManagerFactory & _contentManagerFactory, Host _server);
    bool stopClients(unsigned clientId);
    // TODO - collect statistics
protected:
private:
    std::map<unsigned, std::vector<std::unique_ptr<ContentManager>>> workers;
    std::mutex lock;
};