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

#include <map>
#include <vector>
#include "protocolfactory.h"
#include "contentmanagerfactory.h"
#include "host.h"

class Client {
public:
    Client();
    virtual ~Client();
    bool startClients(unsigned clientId, unsigned num_clients, ProtocolFactory & _protocolFactory, ContentManagerFactory & _contentManagerFactory, Host _server);
    bool stopClients(unsigned clientId);
    // TODO - collect statistics
protected:
private:
    std::map<unsigned, std::vector<std::unique_ptr<ContentManager>>> workers;
    std::mutex lock;
};
