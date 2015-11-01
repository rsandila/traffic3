/*
 * Copyright (C) 2003 Robert Sandilands (Pty) Ltd.
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

Server::Server(ProtocolFactory & _protocolFactory, ContentManagerFactory & contentManagerFactory) :
        protocolFactory(_protocolFactory), contentFactory(contentManagerFactory) {
}

Server::~Server() {
    for (auto it = listeners.begin(); it != listeners.end(); ++it) {
        (*it)->Stop();
    }
}

bool Server::addPort(Host & host) {
    std::unique_lock<std::mutex> lck(lock);
    // don't add a second port that is the same
    for (auto it = listeners.cbegin(); it != listeners.cend(); ++it) {
        if (*(*it) == host) {
            return false;
        }
    }
    listeners.push_back(std::unique_ptr<Listener>(new Listener(host, protocolFactory, contentFactory)));
    return true;
}


bool Server::stopPort(Host & host) {
    std::unique_lock<std::mutex> lck(lock);
    for (auto it = listeners.begin(); it != listeners.end(); ++it) {
        if (*(*it) == host) {
            bool retVal = (*it)->Stop();
            listeners.erase(it);
            return retVal;
        }
    }
    return false;
}

const std::vector<Host> Server::getPorts() const noexcept {
    std::unique_lock<std::mutex> lck(lock);
    std::vector<Host> retVal;
    for (auto it = listeners.cbegin(); it != listeners.cend(); ++it) {
        retVal.push_back((*it)->getHost());
    }
    return std::move(retVal);
}
