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

class RestState {
public:
    int getNumClients() noexcept;
    int getNumServers() noexcept;
    
    int startClient(unsigned clientId, unsigned num_clients, ProtocolFactory & _protocolFactory, ContentManagerFactory & _contentManagerFactory, Host & _server);
    bool stopClient(unsigned clientId);
    
    int startServer(unsigned portId, Host & host, ProtocolFactory & protocolFactory, std::shared_ptr<ContentManagerFactory> & contentManagerFactory);
    bool stopServer(unsigned portId);
    
    long long getClientNumWritten() const noexcept;
    long long getClientNumRead() const noexcept;
    long long getServerNumWritten() const noexcept;
    long long getServerNumRead() const noexcept;
    
protected:
private:
    Client client;
    Server server;
};