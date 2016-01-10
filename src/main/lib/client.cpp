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
#include "client.h"

Client::~Client() {
    for (auto it = workers.begin(); it != workers.end(); it++) {
        for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++) {
            (*it2)->Stop();
        }
    }
}

bool Client::startClients(unsigned clientId, unsigned num_clients, ProtocolFactory _protocolFactory, ContentManagerFactory  _contentManagerFactory, Host _server) {
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
    for (unsigned cnt = 0; cnt < clients.size(); cnt++) {
        if (!clients[cnt]->Start()) {
            return false;
        }
    }
    workers[clientId] = std::move(clients);
    return workers[clientId].size() == num_clients;
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

int Client::getNumClients() noexcept {
    std::unique_lock<std::mutex> lck(lock);
    return workers.size();
}

long long Client::getNumBytesRead() const noexcept {
    long long total = 0;
    for (const auto & client: workers) {
        for (const auto & one: client.second) {
            total += one->getBytesRead();
        }
    }
    return total;
}

long long Client::getNumBytesWritten() const noexcept {
    long long total = 0;
    for (const auto & client: workers) {
        for (const auto & one: client.second) {
            total += one->getBytesWritten();
        }
    }
    return total;
}

nlohmann::json Client::toJson() const noexcept {
    nlohmann::json returnValue;
    
    for (const auto & it: workers) {
        std::vector<nlohmann::json> returnArray;
        
        for (const auto & it2: it.second) {
            returnArray.push_back(it2->toJson());
        }
        returnValue[it.first] = returnArray;
    }
    return std::move(returnValue);
}