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
#include "rest/rest_server.h"

TEST_CASE("RestServer", "[rest][server]") {
    SECTION("URI not matching: short") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getServerJson);
        
        RestServer server("/testserver", *restState);
        RestRequest request(RestRequestType::RRT_GET, "/testserve", "1.0");
        
        REQUIRE(server.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>()) == std::vector<char>());
    }
    SECTION("URI not matching: long") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getServerJson);
        
        RestServer server("/testserver", *restState);
        RestRequest request(RestRequestType::RRT_GET, "/testserver2", "1.0");
        
        REQUIRE(server.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>()) == std::vector<char>());
    }
    SECTION("Invalid Type: POST") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getServerJson);
        
        RestServer server("/testserver", *restState);
        RestRequest request(RestRequestType::RRT_POST, "/testserver", "1.0");
        
        REQUIRE(server.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>()) == std::vector<char>());
    }
    SECTION("Invalid Type: HEAD") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getServerJson);
        
        RestServer server("/testserver", *restState);
        RestRequest request(RestRequestType::RRT_HEAD, "/testserver", "1.0");
        
        REQUIRE(server.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>()) == std::vector<char>());
    }
    SECTION("Invalid Type: TRACE") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getServerJson);
        
        RestServer server("/testserver", *restState);
        RestRequest request(RestRequestType::RRT_TRACE, "/testserver", "1.0");
        
        REQUIRE(server.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>()) == std::vector<char>());
    }
    SECTION("Invalid Type: OPTIONS") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getServerJson);
        
        RestServer server("/testserver", *restState);
        RestRequest request(RestRequestType::RRT_OPTIONS, "/testserver", "1.0");
        
        REQUIRE(server.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>()) == std::vector<char>());
    }
    SECTION("Invalid Type: CONNECT") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getServerJson);
        
        RestServer server("/testserver", *restState);
        RestRequest request(RestRequestType::RRT_CONNECT, "/testserver", "1.0");
        
        REQUIRE(server.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>()) == std::vector<char>());
    }
    // TODO - all the GET variants
    // TOOD - all the DELETE variants
    // TODO - all the PUT variants
}
