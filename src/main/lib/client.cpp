#include "client.h"

Client::Client() {
    
}

Client::~Client() {
    
}

bool Client::startClients(unsigned clientId, unsigned num_clients, ProtocolFactory & _protocolFactory, ContentManagerFactory & _contentManagerFactory,
                          Host _server) {
    std::unique_lock<std::mutex> lck(lock);
    if (workers.find(clientId) != workers.end()) {
        return false;
    }
    std::vector<std::unique_ptr<ContentManager>> clients;
    for (unsigned cnt = 0; cnt < num_clients; cnt++) {
        std::unique_ptr<Protocol> protocol(std::move(_protocolFactory.createProtocol()));
        if (protocol->connect(_server)) {
            clients.push_back(std::move(_contentManagerFactory.createContentManager(std::move(protocol), false)));
        }
    }
    workers[clientId] = std::move(clients);
    return true;
}

bool Client::stopClients(unsigned clientId) {
    std::unique_lock<std::mutex> lck(lock);
    auto client = workers.find(clientId);
    if (client == workers.end()) {
        return false;
    }
    workers.erase(client);
    return true;
}
