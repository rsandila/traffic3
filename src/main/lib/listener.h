#pragma once

#include <functional>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>
#include "contentmanagerfactory.h"
#include "protocolfactory.h"

class Listener {
public:
    Listener(const Host & _host, ProtocolFactory & protocolFactory, ContentManagerFactory & contentManagerFactory) :
            host(_host), protocol(protocolFactory.createProtocol()), _contentManagerFactory(contentManagerFactory),
            thread(std::thread(std::bind(&Listener::listen, this))) {
    }
    Listener(Listener && other) : host(other.host), protocol(std::move(other.protocol)),
            _contentManagerFactory(other._contentManagerFactory), contentManagers(std::move(other.contentManagers)) {
        thread = std::move(other.thread);
    }
    Listener & operator=(Listener&& other) {
        host = std::move(other.host);
        protocol = std::move(other.protocol);
        _contentManagerFactory = std::move(other._contentManagerFactory);
        contentManagers = std::move(other.contentManagers);
        thread = std::move(other.thread);
        return *this;
    }
    bool operator==(const Host & other) const {
        return host == other;
    }
    const Host & getHost() const {
        return host;
    }
    virtual ~Listener() {
        Stop();
    }
    bool Stop() {
        if (protocol->getState() == Protocol::ProtocolState::CLOSED) {
            return false;
        }
        protocol->close();
        for (auto manager = contentManagers.begin(); manager != contentManagers.end(); manager++) {
            (*manager)->Stop();
        }
        thread.join();
        return true;
    }
protected:
    void listen() {
        if (protocol->listen(host)) {
            std::unique_ptr<Protocol> newProtocol = protocol->waitForNewConnection();
            if (newProtocol->getState() == Protocol::ProtocolState::OPEN) {
                std::unique_ptr<ContentManager> tempContentManager =_contentManagerFactory.createContentManager(std::move(newProtocol));
                // TODO - configure contentManager
                contentManagers.push_back(std::move(tempContentManager));
            }
        } else {
            std::ostringstream ostr;
            ostr << "Unable to listen on " << host << std::endl;
            throw new std::runtime_error(ostr.str());
        }
    }
private:
    Host host;
    std::unique_ptr<Protocol> protocol;
    ContentManagerFactory & _contentManagerFactory;
    std::vector<std::unique_ptr<ContentManager>> contentManagers;
    std::thread thread;

    Listener(const Listener &) = delete;
    Listener & operator=(const Listener &) = delete;
 };


