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

#include <vector>
#include <thread>
#include <mutex>
#include "listener.h"
#include "protocolfactory.h"
#include "contentmanagerfactory.h"

class Server {
public:
    Server(ProtocolFactory & protocolFactory, ContentManagerFactory & contentManagerFactory);
    virtual ~Server();
    bool addPort(Host & host);
    bool stopPort(Host & host);
    const std::vector<Host> getPorts() const noexcept;
    // TODO - collect statistics
protected:
private:
    std::vector<std::unique_ptr<Listener>> listeners;
    ProtocolFactory & protocolFactory;
    ContentManagerFactory & contentFactory;
    mutable std::mutex lock;
};