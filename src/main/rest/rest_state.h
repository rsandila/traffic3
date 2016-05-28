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
#pragma once

#include "lib/client.h"
#include "lib/server.h"
#include "json.hpp"

class RestState {
public:
    virtual int getNumClients() const noexcept;
    virtual int getNumServers() const noexcept;
    
    virtual bool startClient(unsigned clientId, unsigned num_clients, ProtocolFactory & _protocolFactory,
                             ContentManagerFactory & _contentManagerFactory, Host & _server);
    virtual bool stopClient(unsigned clientId);
    
    virtual bool startServer(unsigned portId, Host & host, ProtocolFactory & protocolFactory,
                             std::shared_ptr<ContentManagerFactory> & contentManagerFactory);
    virtual bool stopServer(unsigned portId);
    
    virtual long long getClientNumWritten() const noexcept;
    virtual long long getClientNumRead() const noexcept;
    virtual long long getServerNumWritten() const noexcept;
    virtual long long getServerNumRead() const noexcept;
    virtual nlohmann::json getServerJson() const noexcept;
    virtual nlohmann::json getServerJsonForId(unsigned id) const noexcept;
    virtual nlohmann::json getClientJson() const noexcept;
    virtual nlohmann::json getClientJsonForId(unsigned id) const noexcept;
protected:
private:
    mutable std::mutex clientMutex;
    mutable std::mutex serverMutex;
    
    Client client;
    Server server;
};