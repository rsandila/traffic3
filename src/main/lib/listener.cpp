#include <functional>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>
#include "contentmanagerfactory.h"
#include "protocolfactory.h"
#include "listener.h"

Listener::Listener(const Host & _host, ProtocolFactory & protocolFactory, ContentManagerFactory & contentManagerFactory) :
    host(_host), protocol(protocolFactory.createProtocol()), _contentManagerFactory(contentManagerFactory),
    errorState(false), thread(std::thread(std::bind(&Listener::listen, this))) {
}

Listener::Listener(Listener && other) : host(other.host), protocol(std::move(other.protocol)), errorState(other.errorState),
        _contentManagerFactory(other._contentManagerFactory), contentManagers(std::move(other.contentManagers)) {
    thread = std::move(other.thread);
}

Listener & Listener::operator=(Listener&& other) {
    host = std::move(other.host);
    protocol = std::move(other.protocol);
    _contentManagerFactory = std::move(other._contentManagerFactory);
    contentManagers = std::move(other.contentManagers);
    errorState = other.errorState;
    thread = std::move(other.thread);
    return *this;
}

bool Listener::operator==(const Host & other) const {
    return host == other;
}

const Host & Listener::getHost() const {
    return host;
}

Listener::~Listener() {
    Stop();
}

bool Listener::inErrorState() const noexcept {
    return errorState;
}

bool Listener::Stop() {
    protocol->close();
    for (auto manager = contentManagers.begin(); manager != contentManagers.end(); manager++) {
        (*manager)->Stop();
    }
    thread.join();
    return true;
}

void Listener::listen() {
    if (protocol->listen(host)) {
       std::unique_ptr<Protocol> newProtocol = protocol->waitForNewConnection();
       if (newProtocol.get() != nullptr && newProtocol->getState() == Protocol::ProtocolState::OPEN) {
           std::unique_ptr<ContentManager> tempContentManager =_contentManagerFactory.createContentManager(std::move(newProtocol));
                // TODO - configure contentManager
           if (tempContentManager.get() != nullptr) {
               tempContentManager->Start();
               contentManagers.push_back(std::move(tempContentManager));
           }
       }
    } else {
       errorState = true;
    }
}

