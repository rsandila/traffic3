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
#include "server.h"
#include "listener.h"

Server::~Server() {
    for (auto it = listeners.begin(); it != listeners.end(); ++it) {
        (*it)->Stop();
    }
}

bool Server::addPort(unsigned portId, Host & host, ProtocolFactory & protocolFactory, std::shared_ptr<ContentManagerFactory> & contentManagerFactory) {
    std::unique_lock<std::mutex> lck(lock);
    // don't add a second port that is the same
    for (auto it = listeners.cbegin(); it != listeners.cend(); ++it) {
        if (*(*it) == portId) {
            return false;
        }
    }
    std::unique_ptr<Listener> newListener(new Listener(portId, host, protocolFactory, contentManagerFactory));
    if (!newListener->inErrorState()) {
        listeners.push_back(std::move(newListener));
        return true;
    }
    return false;
}


bool Server::stopPort(unsigned portId) {
    std::unique_lock<std::mutex> lck(lock);
    for (auto it = listeners.begin(); it != listeners.end(); ++it) {
        if (*(*it) == portId) {
            bool retVal = (*it)->Stop();
            listeners.erase(it);
            return retVal;
        }
    }
    return false;
}

const std::vector<unsigned> Server::getPorts() const noexcept {
    std::unique_lock<std::mutex> lck(lock);
    std::vector<unsigned> retVal;
    for (auto it = listeners.cbegin(); it != listeners.cend(); ++it) {
        retVal.push_back((*it)->getPortId());
    }
    return retVal;
}

int Server::getNumServers() noexcept {
    std::unique_lock<std::mutex> lck(lock);
    return listeners.size();
}

long long Server::getNumBytesRead() const noexcept {
    long long total = 0LL;
    for (const auto & port: listeners) {
        total += port->getBytesRead();
    }
    return total;
}

long long Server::getNumBytesWritten() const noexcept {
    long long total = 0LL;
    for (const auto & port: listeners) {
        total += port->getBytesWritten();
    }
    return total;
}

nlohmann::json Server::toJson() const noexcept {
    nlohmann::json returnValue;
    returnValue["numListeners"] = listeners.size();
    
    std::vector<nlohmann::json> listenersJson;
    for (const auto & listener: listeners) {
        listenersJson.push_back(listener->toJson());
    }
    returnValue["listeners"] = listenersJson;
    return returnValue;
}

nlohmann::json Server::toJson(unsigned id) const noexcept {
    nlohmann::json returnValue;
    
    for (const auto & listener: listeners) {
        if (listener->getPortId() == id) {
            returnValue["found"] = true;
            returnValue["listener"] = listener->toJson();
            return returnValue;
        }
    }
    returnValue["found"] = false;
    return returnValue;
}
