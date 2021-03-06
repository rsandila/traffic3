/*
 * Copyright (C) 2015 Robert Sandilands
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 USA.
 */
#include <functional>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>
#include "contentmanager/contentmanagerfactory.h"
#include "protocol/protocolfactory.h"
#include "listener.h"
#include "logging.h"

Listener::Listener(unsigned portId, const Host & _host, ProtocolFactory & protocolFactory,
                   std::shared_ptr<ContentManagerFactory> & contentManagerFactory) : _portId(portId),
        host(_host), protocol(protocolFactory.createProtocol()), _contentManagerFactory(contentManagerFactory),
    errorState(false), thread(std::thread(std::bind(&Listener::listen, this))) {
}

Listener::Listener(Listener && other) : _portId(other._portId), host(other.host),
        protocol(std::move(other.protocol)), _contentManagerFactory(other._contentManagerFactory),
        contentManagers(std::move(other.contentManagers)), errorState(other.errorState), thread(std::move(other.thread)) {
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

bool Listener::operator==(const unsigned portId) const {
    return _portId == portId;
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
    if (protocol.get() != nullptr) {
        protocol->close();
    }
    for (auto manager = contentManagers.begin(); manager != contentManagers.end(); manager++) {
        (*manager)->Stop();
    }
    contentManagers.clear();
    if (thread.joinable()) {
        thread.join();
    }
    return true;
}

void Listener::listen() {
    bool exitLoop = false;
    if (protocol->listen(host, 10)) {
        do {
            std::unique_ptr<Protocol> newProtocol = protocol->waitForNewConnection();
            if (newProtocol.get() != nullptr && newProtocol->getState() == Protocol::ProtocolState::OPEN) {
                std::unique_ptr<ContentManager> tempContentManager =_contentManagerFactory->createContentManager(std::move(newProtocol), true);
                if (tempContentManager.get() != nullptr) {
                    tempContentManager->Start();
                    contentManagers.push_back(std::move(tempContentManager));
                }
            } else {
                LOG(WARNING) << " exiting listening loop for " << host << std::endl;
                exitLoop = true;
            }
        } while (!exitLoop);
    } else {
        LOG(WARNING) << " listen on " << host << " exiting because of listen error " << std::endl;
        errorState = true;
    }
    
}

unsigned Listener::getPortId() const noexcept {
    return _portId;
}

long long Listener::getBytesRead() const noexcept {
    if (protocol.get()) {
        long long returnValue = protocol->getBytesRead();
        for (const auto & contentManager: contentManagers) {
            returnValue += contentManager.get()->getBytesRead();
        }
        return returnValue;
    } else {
        return 0LL;
    }
}

long long Listener::getBytesWritten() const noexcept {
    if (protocol.get()) {
        long long returnValue = protocol->getBytesWritten();
        for (const auto & contentManager: contentManagers) {
            returnValue += contentManager.get()->getBytesWritten();
        }
        return returnValue;
    } else {
        return 0LL;
    }
}

nlohmann::json Listener::toJson() const noexcept {
    nlohmann::json returnValue;
    
    returnValue["id"] = _portId;
    returnValue["host"] = host.toJson();
    returnValue["protocol"] = protocol->toJson();
    returnValue["contentManagerFactory"] = _contentManagerFactory->toJson();
    returnValue["numContentManagers"] = contentManagers.size();
    
    std::vector<nlohmann::json> contentManagersJson;
    for (const auto & contentManager: contentManagers) {
        contentManagersJson.push_back(contentManager->toJson());
    }
    returnValue["contentManagers"] = contentManagersJson;
    returnValue["errorState"] = errorState;
    
    return returnValue;
}
