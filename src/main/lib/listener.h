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

#include "contentmanagerfactory.h"
#include "protocolfactory.h"

class Listener {
public:
    Listener(const Host & _host, ProtocolFactory & protocolFactory, ContentManagerFactory & contentManagerFactory);
    Listener(Listener && other);
    virtual ~Listener();
    Listener & operator=(Listener&& other);
    bool operator==(const Host & other) const;
    const Host & getHost() const;
    bool inErrorState() const noexcept;
    bool Stop();
protected:
    void listen();
private:
    Host host;
    std::unique_ptr<Protocol> protocol;
    ContentManagerFactory & _contentManagerFactory;
    std::vector<std::unique_ptr<ContentManager>> contentManagers;
    bool errorState;
    std::thread thread;
    
    Listener(const Listener &) = delete;
    Listener & operator=(const Listener &) = delete;
 };


