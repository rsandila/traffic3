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

class ProtocolUDP : public Protocol {
public:
	ProtocolUDP();
	virtual ~ProtocolUDP();
	virtual bool read(std::vector<char> & data, bool allowPartialRead, Host & hostState) override;
	virtual bool write(const std::vector<char> & data, const Host & hostState) override;
protected:
	ProtocolUDP(int socket, socklen_t len, const struct sockaddr * addr, bool isIPV4);
	ProtocolUDP(ProtocolUDP && other);
	virtual bool realListen(const Host & localHost, const int af, const struct sockaddr * addr, const int addr_len, const int backlog);
	virtual bool realConnect(const Host & localHost, const int af, const struct sockaddr * addr, const int addr_len);
private:
	ProtocolUDP(const ProtocolUDP &) = delete;
	ProtocolUDP & operator=(const ProtocolUDP &) = delete;
};

