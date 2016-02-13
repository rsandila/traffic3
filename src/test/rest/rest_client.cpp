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

// TODO
/*
 RRT_GET - v,
 RRT_PUT - v,
 RRT_DELETE - v,
 */
#include "catch.hpp"
#include "hippomocks.h"
#include "rest/rest_client.h"

TEST_CASE("RestClient", "[rest][server]") {
    SECTION("URI not matching: short") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getClientJson);

        RestClient client("/testclient", *restState);
        RestRequest request(RestRequestType::RRT_GET, "/testclien", "1.0");

        REQUIRE(client.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>()) == std::vector<char>());
    }
    SECTION("URI not matching: long") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getClientJson);

        RestClient client("/testclient", *restState);
        RestRequest request(RestRequestType::RRT_GET, "/testclient2", "1.0");
        
        REQUIRE(client.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>()) == std::vector<char>());
    }
    SECTION("Invalid Type: POST") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getClientJson);

        RestClient client("/testclient", *restState);
        RestRequest request(RestRequestType::RRT_POST, "/testclient", "1.0");

        REQUIRE(client.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>()) == std::vector<char>());
    }
    SECTION("Invalid Type: HEAD") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getClientJson);
        
        RestClient client("/testclient", *restState);
        RestRequest request(RestRequestType::RRT_HEAD, "/testclient", "1.0");
        
        REQUIRE(client.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>()) == std::vector<char>());
    }
    SECTION("Invalid Type: TRACE") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getClientJson);
        
        RestClient client("/testclient", *restState);
        RestRequest request(RestRequestType::RRT_TRACE, "/testclient", "1.0");
        
        REQUIRE(client.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>()) == std::vector<char>());
    }
    SECTION("Invalid Type: OPTIONS") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getClientJson);
        
        RestClient client("/testclient", *restState);
        RestRequest request(RestRequestType::RRT_OPTIONS, "/testclient", "1.0");
        
        REQUIRE(client.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>()) == std::vector<char>());
    }
    SECTION("Invalid Type: CONNECT") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getClientJson);
        
        RestClient client("/testclient", *restState);
        RestRequest request(RestRequestType::RRT_CONNECT, "/testclient", "1.0");
        
        REQUIRE(client.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>()) == std::vector<char>());
    }
    SECTION("Valid Type: GET unknown parameters") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.ExpectCall(restState, RestState::getClientJson).Return(std::vector<char> {'a'});
        
        RestClient client("/testclient", *restState);
        RestRequest request(RestRequestType::RRT_GET, "/testclient?boo=123", "1.0");
        
        REQUIRE(client.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>()) != std::vector<char>());
    }
    // TODO - test GET with no id
    // TODO - test GET with id that is not a number
    // TODO - test GET with id with no value
    // TODO - test GET with a valud id
    // TODO - test DELETE
    // TODO - test POST
}