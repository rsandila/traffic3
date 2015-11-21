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
#ifndef _MSC_VER
#include <arpa/inet.h>
#else
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#endif
#include <thread>
#include "commonheaders.h"
#include "logging.h"

CommonHeaders::CommonHeaders() {
}

CommonHeaders::~CommonHeaders() {
}

bool CommonHeaders::read(std::unique_ptr<Protocol> & protocol, std::vector<char> & content, Host & hostState) {
    std::vector<char> signature(4 + sizeof(uint32_t));
    if (protocol->read(signature, false, hostState)) {
        if (signature.size() == (4 + sizeof(uint32_t)) &&
            memcmp(&signature[0], "TRAF", 4) == 0) {
                // next 4 bytes is length in network order and includes the header
            uint32_t length = ntohl(*(uint32_t *)(&(signature[4])));
			if (length < 4 + sizeof(uint32_t)) {
				return false; // invalid header
			}
            content.resize(length - (4 + sizeof(uint32_t)));
            if (content.size() == 0) {
                return true;
            }
            LOG(DEBUG) << std::this_thread::get_id() << " reading " << length << " " << content.size() << " bytes" << std::endl;
            return protocol->read(content, false, hostState);
        } else {
            LOG(WARNING) << std::this_thread::get_id() << " Invalid signature received " << signature.size() << " " << &signature[0] << std::endl;
        }
    }
    return false;
}

bool CommonHeaders::write(std::unique_ptr<Protocol> & protocol, const std::vector<char> & content, const Host & hostState) {
    std::vector<char> signature(4 + sizeof(uint32_t));
    memcpy(&signature[0], "TRAF", 4);
    uint32_t length = htonl(8 + content.size());
    memcpy(&signature[4], &length, sizeof(uint32_t));
    if (protocol->write(signature, hostState)) {
        return protocol->write(content, hostState);
    }
    return false;
}

unsigned CommonHeaders::getVersion() const {
    return 2;
}
