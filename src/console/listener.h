#pragma once

#include <functional>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <thread>
#include "contentmanagerfactory.h"
#include "protocol.h"

class Listener {
public:
    Listener(const Host & _host, Protocol & protocol, ContentManagerFactory & contentManagerFactory) : host(_host),
            _protocol(protocol), _contentManagerFactory(contentManagerFactory),
            thread(std::thread(std::bind(&Listener::listen, this))) {
    }
    Listener(Listener && other) : host(other.host), _protocol(other._protocol),
            _contentManagerFactory(other._contentManagerFactory), contentManagers(other.contentManagers) {
        thread = std::move(other.thread);
    }
    Listener & operator=(Listener&& other) {
        host = std::move(other.host);
        _protocol = std::move(other._protocol);
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
        if (_protocol.getState() == Protocol::ProtocolState::CLOSED) {
            return false;
        }
        _protocol.close();
        for (auto manager: contentManagers) {
            manager.Stop();
        }
        thread.join();
        return true;
    }
protected:
    void listen() {
        if (_protocol.listen(host)) {
            Protocol newProtocol = _protocol.waitForNewConnection();
            if (newProtocol.getState() == Protocol::ProtocolState::OPEN) {
                contentManagers.push_back(_contentManagerFactory.createContentManager(newProtocol));
            }
        } else {
            std::ostringstream ostr;
            ostr << "Unable to listen on " << host << std::endl;
            throw new std::runtime_error(ostr.str());
        }
    }
private:
    Host host;
    Protocol & _protocol;
    ContentManagerFactory & _contentManagerFactory;
    std::vector<ContentManager> contentManagers;
    std::thread thread;

 };


