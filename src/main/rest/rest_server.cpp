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

#include "html_return.h"
#include "rest_server.h"
#include "lib/commonheaders.h"

RestServer::RestServer(const std::string & uriPattern, RestState & _state) : uriBase(uriPattern), state(_state) {
}

std::vector<char> RestServer::handleRequest(const Host & host, const RestRequest & request,
                                            const std::map<std::string, std::string> & headers,
                                            const std::vector<char> & body) {
    UNUSED(host);
    std::vector<char> returnValue;
    if (request.getUri().find(uriBase) == 0) {
        switch (request.getType()) {
            case RestRequestType::RRT_GET:
                // get status
                return handleGetStatus(request, headers, body);
            case RestRequestType::RRT_PUT:
                // create client
                return handleCreateServer(request, headers, body);
            case RestRequestType::RRT_DELETE:
                // stop client
                return handleStopServer(request, headers, body);
            default:
                break;
        }
    }
    return returnValue;
}

std::vector<char> RestServer::handleGetStatus(const RestRequest & request, const std::map<std::string, std::string> & headers, const std::vector<char> & body) const noexcept {
    UNUSED(headers);
    UNUSED(body);
    std::vector<char> returnValue;
    
    if (request.hasParam("id")) {
        // return just the stats for the specific client
        return returnJsonPage(200, "OK", state.getServerJsonForId(std::stoul(request.getParam("id"))).dump());
    } else {
        // return stats for all the clients
        return returnJsonPage(200, "OK", state.getServerJson().dump());
    }
}

std::vector<char> RestServer::handleCreateServer(const RestRequest & request,
                                                 const std::map<std::string, std::string> & headers,
                                                 const std::vector<char> & body) const noexcept {
 
    UNUSED(headers);
    UNUSED(body);
    
    unsigned id = std::stoul(request.getParamWithDefault("id", "0"));
    std::string protocol = request.getParamWithDefault("protocol", "tcp4");
    std::string cm_type = request.getParamWithDefault("contentmanager", "fixed");
    std::string hostName = request.getParamWithDefault("host", "::");
    unsigned port = std::stoul(request.getParam("port"));
    unsigned minimum = std::stoul(request.getParamWithDefault("min", "10"));
    unsigned maximum = std::stoul(request.getParamWithDefault("max", "10000"));
    
    std::unique_ptr<CommonHeaders> commonHeaders(new CommonHeaders);
    ProtocolFactory protocolFactory(convertStringToProtocolType(protocol));
    std::shared_ptr<ContentManagerCustomizer> contentManagerCustomizer(
                                        new ContentManagerCustomizer(minimum, maximum));
    std::shared_ptr<ContentManagerFactory> contentManagerFactory = std::shared_ptr<ContentManagerFactory>(new ContentManagerFactory(convertStringToContentManagerType(cm_type), commonHeaders, contentManagerCustomizer));
    // Server server;
    Host port10000(hostName, port, convertFromProtocolTypeToPreference(convertStringToProtocolType(protocol)));
    
    nlohmann::json returnValue;
    if (state.startServer(id, port10000, protocolFactory, contentManagerFactory)) {
        // return success
        returnValue["result"] = std::string("Ok");
    } else {
        // return failure
        returnValue["result"] = std::string("Failed");
    }
    return std::move(returnJsonPage(200, "OK", returnValue.dump()));
}

std::vector<char> RestServer::handleStopServer(const RestRequest & request,
                                               const std::map<std::string, std::string> & headers,
                                               const std::vector<char> & body) const noexcept {
    UNUSED(headers);
    UNUSED(body);
    nlohmann::json returnValue;
    
    unsigned id = std::stoul(request.getParam("id"));
    if (state.stopServer(id)) {
        // return success
        returnValue["result"] = std::string("Ok");
    } else {
        // return error
        returnValue["result"] = std::string("Failed");
    }
    return std::move(returnJsonPage(200, "OK", returnValue.dump()));;
}
