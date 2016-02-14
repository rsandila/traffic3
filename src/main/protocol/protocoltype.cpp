/*
 * Copyright (C) 2016 Robert Sandilands
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

#include <algorithm>
#include <cctype>
#include <map>
#include "protocoltype.h"

static std::map<std::string, ProtocolType> protocolMap {
    {"tcp4", ProtocolType::TCP4},
    {"tcp6", ProtocolType::TCP6},
    {"udp4", ProtocolType::UDP4},
    {"udp6", ProtocolType::UDP6} };

ProtocolType convertStringToProtocolType(const std::string & name) {
    std::string lname = name;
    std::for_each(name.begin(), name.end(), [](char a){ return std::tolower(a);});
    if (protocolMap.find(lname) == protocolMap.end()) {
        throw std::invalid_argument("Invalid protocol specified");
    }
    return protocolMap[lname];
}

const std::string protocolTypeToString(const ProtocolType type) {
    for (const auto & it: protocolMap) {
        if (it.second == type) {
            return it.first;
        }
    }
    return "tcp4"; // should probably throw some exception
}
