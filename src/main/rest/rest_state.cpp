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
#include "rest/rest_state.h"

int RestState::getNumClients() const noexcept {
    std::unique_lock<std::mutex> lck(clientMutex);
    
    return client.getNumClients();
}

int RestState::getNumServers() const noexcept {
    std::unique_lock<std::mutex> lck(serverMutex);
    return server.getNumServers();
}

bool RestState::startClient(unsigned clientId, unsigned num_clients, ProtocolFactory & _protocolFactory,
                           ContentManagerFactory & _contentManagerFactory, Host & _server) {
    std::unique_lock<std::mutex> lck(clientMutex);
    return client.startClients(clientId, num_clients, _protocolFactory, _contentManagerFactory, _server);
}

bool RestState::stopClient(unsigned clientId) {
    std::unique_lock<std::mutex> lck(clientMutex);
    return client.stopClients(clientId);
}

bool RestState::startServer(unsigned portId, Host & host, ProtocolFactory & protocolFactory,
                           std::shared_ptr<ContentManagerFactory> & contentManagerFactory) {
    std::unique_lock<std::mutex> lck(serverMutex);
    return server.addPort(portId, host, protocolFactory, contentManagerFactory);
}

bool RestState::stopServer(unsigned portId) {
    std::unique_lock<std::mutex> lck(serverMutex);
    return server.stopPort(portId);
}

long long RestState::getClientNumWritten() const noexcept {
    std::unique_lock<std::mutex> lck(clientMutex);
    return client.getNumBytesWritten();
}

long long RestState::getClientNumRead() const noexcept {
    std::unique_lock<std::mutex> lck(clientMutex);
    return client.getNumBytesRead();
}

long long RestState::getServerNumWritten() const noexcept {
    std::unique_lock<std::mutex> lck(serverMutex);
    return server.getNumBytesWritten();
}

long long RestState::getServerNumRead() const noexcept {
    std::unique_lock<std::mutex> lck(serverMutex);
    return server.getNumBytesRead();
}

nlohmann::json RestState::getServerJson() const noexcept {
    std::unique_lock<std::mutex> lck(serverMutex);
    return server.toJson();
}

nlohmann::json RestState::getServerJsonForId(unsigned id) const noexcept {
    std::unique_lock<std::mutex> lck(serverMutex);
    return server.toJson(id);
}

nlohmann::json RestState::getClientJson() const noexcept {
    std::unique_lock<std::mutex> lck(clientMutex);
    return client.toJson();
}

nlohmann::json RestState::getClientJsonForId(unsigned id) const noexcept {
    std::unique_lock<std::mutex> lck(clientMutex);
    return client.toJson(id);
}
