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

#include "lib/common.h"
#include "rest_client.h"

RestClient::RestClient(const std::string & uriPattern, RestState & _state) : state(_state) {
}

std::vector<char> RestClient::handleRequest(const Host & host, const RestRequest & request, const std::map<std::string,
                                            std::string> & headers, const std::vector<char> & body) {
    UNUSED(host);
    std::vector<char> returnValue;
    if (request.getUri().find("/client") == 0) {
        switch (request.getType()) {
            case RestRequestType::RRT_GET:
                // get status
                return handleGetStatus(request, headers, body);
            case RestRequestType::RRT_PUT:
                // create client
                return handleCreateClient(request, headers, body);
            case RestRequestType::RRT_DELETE:
                // stop client
                return handleStopClient(request, headers, body);
            default:
                break;
        }
    }
    return returnValue;
}

std::vector<char> RestClient::handleGetStatus(const RestRequest & request,
                                              const std::map<std::string, std::string> & headers,
                                              const std::vector<char> & body) const noexcept {
    UNUSED(headers);
    UNUSED(body);
    std::vector<char> returnValue;
    // TODO - return the list of clients with their descriptions
    return returnValue;
}

std::vector<char> RestClient::handleCreateClient(const RestRequest & request,
                                                 const std::map<std::string, std::string> & headers,
                                                 const std::vector<char> & body) const noexcept {
    UNUSED(headers);
    UNUSED(body);
    std::vector<char> returnValue;
    // TODO
    return returnValue;
}

std::vector<char> RestClient::handleStopClient(const RestRequest & request,
                                               const std::map<std::string, std::string> & headers,
                                               const std::vector<char> & body) const noexcept {
    UNUSED(headers);
    UNUSED(body);
    std::vector<char> returnValue;
    // TODO
    return returnValue;
}
