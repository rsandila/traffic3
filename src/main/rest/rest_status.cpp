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

#include <string>
#include <cstring>
#include "lib/common.h"
#include "rest/rest_status.h"
#include "rest/rest_state.h"
#include "json.hpp"
#include "rest/html_return.h"

RestStatus::RestStatus(const std::string & _uriPattern, RestState & _state) : uriPattern(_uriPattern), state(_state) {
}

std::vector<char> RestStatus::handleRequest(const Host & host, const RestRequest & request, const std::map<std::string, std::string> & headers, const std::vector<char> & body) {
    UNUSED(host);
    UNUSED(request);
    UNUSED(headers);
    UNUSED(body);
    
    if (request.getType() != RestRequestType::RRT_GET) {
        // not me, return empty
        return std::vector<char>();
    }

    if (request.getUri().substr(0, uriPattern.size()) != uriPattern) {
        // not me, return empty
        return std::vector<char>();
    }
    
    if (request.getUri() == uriPattern) {
        return basicStatus();
    }
    std::string rest = request.getUri().substr(uriPattern.size());
    if (rest == "/supports/protocol") {
        return supportedProtocols();
    }
    if (rest == "/supports/contentmanager") {
        return supportedContentManagers();
    }
    
    return std::vector<char>();
}

std::vector<char> RestStatus::basicStatus() const noexcept {
    nlohmann::json returnValue;
    
    returnValue["numClients"] = state.getNumClients();
    returnValue["numServers"] = state.getNumServers();
    returnValue["serverNumRead"] = state.getServerNumRead();
    returnValue["serverNumWriten"] = state.getServerNumWritten();
    returnValue["clientNumRead"] = state.getClientNumRead();
    returnValue["clientNumWritten"] = state.getClientNumWritten();
    std::string returnBody = returnValue.dump(4);
    return returnJsonPage(200, "OK", returnValue.dump(0));
}

std::vector<char> RestStatus::supportedProtocols() const noexcept {
 
    return returnJsonPage(200, "OK", protocolTypesToJson().dump());
}

std::vector<char> RestStatus::supportedContentManagers() const noexcept {

    return returnJsonPage(200, "OK", contentManagerTypesToJson().dump());
}

