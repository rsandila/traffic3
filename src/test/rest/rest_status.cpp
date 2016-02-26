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

#include "catch.hpp"
#include "hippomocks.h"
#include "rest/rest_status.h"

TEST_CASE("RestStatus", "[rest][status][server][client]") {
    SECTION("PUT") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getClientJson);
        mocks.NeverCall(restState, RestState::getNumClients);
        mocks.NeverCall(restState, RestState::getNumServers);
        mocks.NeverCall(restState, RestState::startClient);
        mocks.NeverCall(restState, RestState::stopClient);
        mocks.NeverCall(restState, RestState::startServer);
        mocks.NeverCall(restState, RestState::stopServer);
        mocks.NeverCall(restState, RestState::getClientNumRead);
        mocks.NeverCall(restState, RestState::getClientNumWritten);
        mocks.NeverCall(restState, RestState::getServerNumRead);
        mocks.NeverCall(restState, RestState::getServerNumWritten);
        mocks.NeverCall(restState, RestState::getServerJson);
        mocks.NeverCall(restState, RestState::getServerJsonForId);
        mocks.NeverCall(restState, RestState::getClientJson);
        mocks.NeverCall(restState, RestState::getClientJsonForId);
        
        RestStatus status("/test", *restState);
        RestRequest request(RestRequestType::RRT_PUT, "/test", "1.0");
        
        REQUIRE(status.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>()) == std::vector<char>());
    }
    SECTION("POST") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getClientJson);
        mocks.NeverCall(restState, RestState::getNumClients);
        mocks.NeverCall(restState, RestState::getNumServers);
        mocks.NeverCall(restState, RestState::startClient);
        mocks.NeverCall(restState, RestState::stopClient);
        mocks.NeverCall(restState, RestState::startServer);
        mocks.NeverCall(restState, RestState::stopServer);
        mocks.NeverCall(restState, RestState::getClientNumRead);
        mocks.NeverCall(restState, RestState::getClientNumWritten);
        mocks.NeverCall(restState, RestState::getServerNumRead);
        mocks.NeverCall(restState, RestState::getServerNumWritten);
        mocks.NeverCall(restState, RestState::getServerJson);
        mocks.NeverCall(restState, RestState::getServerJsonForId);
        mocks.NeverCall(restState, RestState::getClientJson);
        mocks.NeverCall(restState, RestState::getClientJsonForId);
        
        RestStatus status("/test", *restState);
        RestRequest request(RestRequestType::RRT_POST, "/test", "1.0");
        
        REQUIRE(status.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>()) == std::vector<char>());
    }
    SECTION("HEAD") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getClientJson);
        mocks.NeverCall(restState, RestState::getNumClients);
        mocks.NeverCall(restState, RestState::getNumServers);
        mocks.NeverCall(restState, RestState::startClient);
        mocks.NeverCall(restState, RestState::stopClient);
        mocks.NeverCall(restState, RestState::startServer);
        mocks.NeverCall(restState, RestState::stopServer);
        mocks.NeverCall(restState, RestState::getClientNumRead);
        mocks.NeverCall(restState, RestState::getClientNumWritten);
        mocks.NeverCall(restState, RestState::getServerNumRead);
        mocks.NeverCall(restState, RestState::getServerNumWritten);
        mocks.NeverCall(restState, RestState::getServerJson);
        mocks.NeverCall(restState, RestState::getServerJsonForId);
        mocks.NeverCall(restState, RestState::getClientJson);
        mocks.NeverCall(restState, RestState::getClientJsonForId);
        
        RestStatus status("/test", *restState);
        RestRequest request(RestRequestType::RRT_HEAD, "/test", "1.0");
        
        REQUIRE(status.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>()) == std::vector<char>());
    }
    SECTION("DELETE") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getClientJson);
        mocks.NeverCall(restState, RestState::getNumClients);
        mocks.NeverCall(restState, RestState::getNumServers);
        mocks.NeverCall(restState, RestState::startClient);
        mocks.NeverCall(restState, RestState::stopClient);
        mocks.NeverCall(restState, RestState::startServer);
        mocks.NeverCall(restState, RestState::stopServer);
        mocks.NeverCall(restState, RestState::getClientNumRead);
        mocks.NeverCall(restState, RestState::getClientNumWritten);
        mocks.NeverCall(restState, RestState::getServerNumRead);
        mocks.NeverCall(restState, RestState::getServerNumWritten);
        mocks.NeverCall(restState, RestState::getServerJson);
        mocks.NeverCall(restState, RestState::getServerJsonForId);
        mocks.NeverCall(restState, RestState::getClientJson);
        mocks.NeverCall(restState, RestState::getClientJsonForId);
        
        RestStatus status("/test", *restState);
        RestRequest request(RestRequestType::RRT_DELETE, "/test", "1.0");
        
        REQUIRE(status.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>()) == std::vector<char>());
    }
    SECTION("TRACE") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getClientJson);
        mocks.NeverCall(restState, RestState::getNumClients);
        mocks.NeverCall(restState, RestState::getNumServers);
        mocks.NeverCall(restState, RestState::startClient);
        mocks.NeverCall(restState, RestState::stopClient);
        mocks.NeverCall(restState, RestState::startServer);
        mocks.NeverCall(restState, RestState::stopServer);
        mocks.NeverCall(restState, RestState::getClientNumRead);
        mocks.NeverCall(restState, RestState::getClientNumWritten);
        mocks.NeverCall(restState, RestState::getServerNumRead);
        mocks.NeverCall(restState, RestState::getServerNumWritten);
        mocks.NeverCall(restState, RestState::getServerJson);
        mocks.NeverCall(restState, RestState::getServerJsonForId);
        mocks.NeverCall(restState, RestState::getClientJson);
        mocks.NeverCall(restState, RestState::getClientJsonForId);
        
        RestStatus status("/test", *restState);
        RestRequest request(RestRequestType::RRT_TRACE, "/test", "1.0");
        
        REQUIRE(status.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>()) == std::vector<char>());
    }
    SECTION("OPTIONS") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getClientJson);
        mocks.NeverCall(restState, RestState::getNumClients);
        mocks.NeverCall(restState, RestState::getNumServers);
        mocks.NeverCall(restState, RestState::startClient);
        mocks.NeverCall(restState, RestState::stopClient);
        mocks.NeverCall(restState, RestState::startServer);
        mocks.NeverCall(restState, RestState::stopServer);
        mocks.NeverCall(restState, RestState::getClientNumRead);
        mocks.NeverCall(restState, RestState::getClientNumWritten);
        mocks.NeverCall(restState, RestState::getServerNumRead);
        mocks.NeverCall(restState, RestState::getServerNumWritten);
        mocks.NeverCall(restState, RestState::getServerJson);
        mocks.NeverCall(restState, RestState::getServerJsonForId);
        mocks.NeverCall(restState, RestState::getClientJson);
        mocks.NeverCall(restState, RestState::getClientJsonForId);
        
        RestStatus status("/test", *restState);
        RestRequest request(RestRequestType::RRT_OPTIONS, "/test", "1.0");
        
        REQUIRE(status.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>()) == std::vector<char>());
    }
    SECTION("GET, short path") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getClientJson);
        mocks.NeverCall(restState, RestState::getNumClients);
        mocks.NeverCall(restState, RestState::getNumServers);
        mocks.NeverCall(restState, RestState::startClient);
        mocks.NeverCall(restState, RestState::stopClient);
        mocks.NeverCall(restState, RestState::startServer);
        mocks.NeverCall(restState, RestState::stopServer);
        mocks.NeverCall(restState, RestState::getClientNumRead);
        mocks.NeverCall(restState, RestState::getClientNumWritten);
        mocks.NeverCall(restState, RestState::getServerNumRead);
        mocks.NeverCall(restState, RestState::getServerNumWritten);
        mocks.NeverCall(restState, RestState::getServerJson);
        mocks.NeverCall(restState, RestState::getServerJsonForId);
        mocks.NeverCall(restState, RestState::getClientJson);
        mocks.NeverCall(restState, RestState::getClientJsonForId);
        
        RestStatus status("/test", *restState);
        RestRequest request(RestRequestType::RRT_GET, "/tes", "1.0");
        
        REQUIRE(status.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>()) == std::vector<char>());
    }
    SECTION("GET, long path") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getClientJson);
        mocks.NeverCall(restState, RestState::getNumClients);
        mocks.NeverCall(restState, RestState::getNumServers);
        mocks.NeverCall(restState, RestState::startClient);
        mocks.NeverCall(restState, RestState::stopClient);
        mocks.NeverCall(restState, RestState::startServer);
        mocks.NeverCall(restState, RestState::stopServer);
        mocks.NeverCall(restState, RestState::getClientNumRead);
        mocks.NeverCall(restState, RestState::getClientNumWritten);
        mocks.NeverCall(restState, RestState::getServerNumRead);
        mocks.NeverCall(restState, RestState::getServerNumWritten);
        mocks.NeverCall(restState, RestState::getServerJson);
        mocks.NeverCall(restState, RestState::getServerJsonForId);
        mocks.NeverCall(restState, RestState::getClientJson);
        mocks.NeverCall(restState, RestState::getClientJsonForId);
        
        RestStatus status("/test", *restState);
        RestRequest request(RestRequestType::RRT_GET, "/test2", "1.0");
        
        REQUIRE(status.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>()) == std::vector<char>());
    }
    SECTION("GET, valid path for stats") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getClientJson);
        mocks.NeverCall(restState, RestState::startClient);
        mocks.NeverCall(restState, RestState::stopClient);
        mocks.NeverCall(restState, RestState::startServer);
        mocks.NeverCall(restState, RestState::stopServer);
        mocks.NeverCall(restState, RestState::getServerJson);
        mocks.NeverCall(restState, RestState::getServerJsonForId);
        mocks.NeverCall(restState, RestState::getClientJson);
        mocks.NeverCall(restState, RestState::getClientJsonForId);
        mocks.ExpectCall(restState, RestState::getNumClients).Return(0);
        mocks.ExpectCall(restState, RestState::getNumServers).Return(0);
        mocks.ExpectCall(restState, RestState::getServerNumRead).Return(0);
        mocks.ExpectCall(restState, RestState::getServerNumWritten).Return(0);
        mocks.ExpectCall(restState, RestState::getClientNumRead).Return(0);
        mocks.ExpectCall(restState, RestState::getClientNumWritten).Return(0);
        
        RestStatus status("/test", *restState);
        RestRequest request(RestRequestType::RRT_GET, "/test", "1.0");
        
        REQUIRE(status.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>()) != std::vector<char>());
    }
    SECTION("GET supported protocols") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getClientJson);
        mocks.NeverCall(restState, RestState::getNumClients);
        mocks.NeverCall(restState, RestState::getNumServers);
        mocks.NeverCall(restState, RestState::startClient);
        mocks.NeverCall(restState, RestState::stopClient);
        mocks.NeverCall(restState, RestState::startServer);
        mocks.NeverCall(restState, RestState::stopServer);
        mocks.NeverCall(restState, RestState::getClientNumRead);
        mocks.NeverCall(restState, RestState::getClientNumWritten);
        mocks.NeverCall(restState, RestState::getServerNumRead);
        mocks.NeverCall(restState, RestState::getServerNumWritten);
        mocks.NeverCall(restState, RestState::getServerJson);
        mocks.NeverCall(restState, RestState::getServerJsonForId);
        mocks.NeverCall(restState, RestState::getClientJson);
        mocks.NeverCall(restState, RestState::getClientJsonForId);
        
        RestStatus status("/test", *restState);
        RestRequest request(RestRequestType::RRT_GET, "/test/supports/protocol", "1.0");
       
        std::vector<char> returnValue = status.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>());
        REQUIRE(returnValue.size() > 0);
        std::string returnString(&returnValue[0], returnValue.size());
        
        REQUIRE(returnString.find("tcp4") != std::string::npos);
        REQUIRE(returnString.find("udp4") != std::string::npos);
        REQUIRE(returnString.find("tcp6") != std::string::npos);
        REQUIRE(returnString.find("udp6") != std::string::npos);
    }
    SECTION("GET, supported protocols, long path") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getClientJson);
        mocks.NeverCall(restState, RestState::getNumClients);
        mocks.NeverCall(restState, RestState::getNumServers);
        mocks.NeverCall(restState, RestState::startClient);
        mocks.NeverCall(restState, RestState::stopClient);
        mocks.NeverCall(restState, RestState::startServer);
        mocks.NeverCall(restState, RestState::stopServer);
        mocks.NeverCall(restState, RestState::getClientNumRead);
        mocks.NeverCall(restState, RestState::getClientNumWritten);
        mocks.NeverCall(restState, RestState::getServerNumRead);
        mocks.NeverCall(restState, RestState::getServerNumWritten);
        mocks.NeverCall(restState, RestState::getServerJson);
        mocks.NeverCall(restState, RestState::getServerJsonForId);
        mocks.NeverCall(restState, RestState::getClientJson);
        mocks.NeverCall(restState, RestState::getClientJsonForId);
        
        RestStatus status("/test", *restState);
        RestRequest request(RestRequestType::RRT_GET, "/test/supports/protocol2", "1.0");
        
        REQUIRE(status.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>()) == std::vector<char>());
    }
    SECTION("GET supported contentmanagers") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getClientJson);
        mocks.NeverCall(restState, RestState::getNumClients);
        mocks.NeverCall(restState, RestState::getNumServers);
        mocks.NeverCall(restState, RestState::startClient);
        mocks.NeverCall(restState, RestState::stopClient);
        mocks.NeverCall(restState, RestState::startServer);
        mocks.NeverCall(restState, RestState::stopServer);
        mocks.NeverCall(restState, RestState::getClientNumRead);
        mocks.NeverCall(restState, RestState::getClientNumWritten);
        mocks.NeverCall(restState, RestState::getServerNumRead);
        mocks.NeverCall(restState, RestState::getServerNumWritten);
        mocks.NeverCall(restState, RestState::getServerJson);
        mocks.NeverCall(restState, RestState::getServerJsonForId);
        mocks.NeverCall(restState, RestState::getClientJson);
        mocks.NeverCall(restState, RestState::getClientJsonForId);
        
        RestStatus status("/test", *restState);
        RestRequest request(RestRequestType::RRT_GET, "/test/supports/contentmanager", "1.0");
        
        std::vector<char> returnValue = status.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>());
        REQUIRE(returnValue.size() > 0);
        std::string returnString(&returnValue[0], returnValue.size());
        
        REQUIRE(returnString.find("fixed") != std::string::npos);
        REQUIRE(returnString.find("echo") != std::string::npos);
        REQUIRE(returnString.find("randomtext") != std::string::npos);
        REQUIRE(returnString.find("randombinary") != std::string::npos);
    }
    SECTION("GET, supported contentmanager, long path") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getClientJson);
        mocks.NeverCall(restState, RestState::getNumClients);
        mocks.NeverCall(restState, RestState::getNumServers);
        mocks.NeverCall(restState, RestState::startClient);
        mocks.NeverCall(restState, RestState::stopClient);
        mocks.NeverCall(restState, RestState::startServer);
        mocks.NeverCall(restState, RestState::stopServer);
        mocks.NeverCall(restState, RestState::getClientNumRead);
        mocks.NeverCall(restState, RestState::getClientNumWritten);
        mocks.NeverCall(restState, RestState::getServerNumRead);
        mocks.NeverCall(restState, RestState::getServerNumWritten);
        mocks.NeverCall(restState, RestState::getServerJson);
        mocks.NeverCall(restState, RestState::getServerJsonForId);
        mocks.NeverCall(restState, RestState::getClientJson);
        mocks.NeverCall(restState, RestState::getClientJsonForId);
        
        RestStatus status("/test", *restState);
        RestRequest request(RestRequestType::RRT_GET, "/test/supports/contentmanager2", "1.0");
        
        REQUIRE(status.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>()) == std::vector<char>());
    }
}