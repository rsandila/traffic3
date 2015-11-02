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
#include <functional>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>
#include "contentmanagerfactory.h"
#include "protocolfactory.h"
#include "listener.h"
#include "logging.h"

Listener::Listener(const Host & _host, ProtocolFactory & protocolFactory, ContentManagerFactory & contentManagerFactory) :
        host(_host), protocol(protocolFactory.createProtocol()), _contentManagerFactory(contentManagerFactory),
    errorState(false), thread(std::thread(std::bind(&Listener::listen, this))) {
}

Listener::Listener(Listener && other) : host(other.host), protocol(std::move(other.protocol)), _contentManagerFactory(other._contentManagerFactory),
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
                std::unique_ptr<ContentManager> tempContentManager =_contentManagerFactory.createContentManager(std::move(newProtocol), true);
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

