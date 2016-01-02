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

#include <sstream>
#include "rest_request.h"

RestRequest::RestRequest(const RestRequestType & type, const std::string & uri, const std::string & version) :
    _type(type), _uri(parseParamsFromUri(uri)), _version(version) {
}

RestRequestType RestRequest::getType() const noexcept {
    return _type;
}

const std::string & RestRequest::getUri() const noexcept {
    return _uri;
}

const std::string & RestRequest::getVersion() const noexcept {
    return _version;
}

const std::string RestRequest::getParam(const std::string & name) const noexcept {
    return getParamWithDefault(name, "");
}

const std::string RestRequest::getParamWithDefault(const std::string & name, const std::string & defaultValue) const noexcept {
    auto retval = params.find(name);
    if (retval == params.end()) {
        return defaultValue;
    }
    return retval->first;
}

const std::string RestRequest::parseParamsFromUri(const std::string & uri) noexcept {
    std::string retval = uri;
    unsigned long pos = retval.find("?");
    if (pos == std::string::npos) {
        return retval;
    }
    std::string parameters = retval.substr(pos + 1);
    retval = uri.substr(0, pos);
    // split on &, then split on =
    std::istringstream istr(parameters);
    std::string param;
    while (std::getline(istr, param, '&')) {
        std::string name, value;
        std::istringstream istr2(param);
        if (std::getline(istr2, name, '=') && std::getline(istr, value, '=')) {
            params[name] = value;
        } else {
            // TODO - do we throw an exception or ignore this?
        }
    }
    return retval;
}