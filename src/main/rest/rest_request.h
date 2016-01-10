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
#include <string>
#include "rest_request_type.h"

class RestRequest {
public:
    RestRequest(const RestRequestType & type, const std::string & uri, const std::string & version);
    RestRequestType getType() const noexcept;
    const std::string & getUri() const noexcept;
    const std::string & getVersion() const noexcept;
    const std::string getParam(const std::string & name) const noexcept;
    const std::string getParamWithDefault(const std::string & name, const std::string & defaultValue) const noexcept;
    bool hasParam(const std::string & name) const noexcept;
protected:
    const std::string parseParamsFromUri(const std::string & uri) noexcept;
private:
    const RestRequestType _type;
    const std::string _version;
    std::map<std::string, std::string> params;
    const std::string _uri;
};