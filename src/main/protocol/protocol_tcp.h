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

#include <mutex>
#include "protocol.h"

class ProtocolTCP : public Protocol {
public:
	ProtocolTCP(const std::string & protocolName);
	virtual ~ProtocolTCP();
	virtual bool read(std::vector<char> & data, bool allowPartialRead, Host & hostState) override;
	virtual bool write(const std::vector<char> & data, const Host & hostState) override;
    virtual bool listen(const Host & localHost, const int backlog) override;
    virtual bool connect(const Host & localHost) override;
protected:
	ProtocolTCP(int socket, socklen_t len, const struct sockaddr * addr, bool isIPV4, const std::string & protocolName);
	ProtocolTCP(ProtocolTCP && other);
private:
	ProtocolTCP(const ProtocolTCP &) = delete;
	ProtocolTCP & operator=(const ProtocolTCP &) = delete;
};

