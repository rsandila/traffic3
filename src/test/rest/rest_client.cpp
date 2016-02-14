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
        nlohmann::json returnJson;
        returnJson["result"] = "Ok";
        
        mocks.ExpectCall(restState, RestState::getClientJson).Return(returnJson);
        
        RestClient client("/testclient", *restState);
        RestRequest request(RestRequestType::RRT_GET, "/testclient?boo=123", "1.0");
        
        REQUIRE(client.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>()) != std::vector<char>());
    }
    SECTION("Valid Type: GET no id") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        nlohmann::json returnJson;
        returnJson["result"] = "Ok";
        mocks.ExpectCall(restState, RestState::getClientJson).Return(returnJson);
        
        RestClient client("/testclient", *restState);
        RestRequest request(RestRequestType::RRT_GET, "/testclient", "1.0");
        
        REQUIRE(client.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>()) != std::vector<char>());
    }
    SECTION("Valid Type: GET id is not a number") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getClientJson);
        
        RestClient client("/testclient", *restState);
        RestRequest request(RestRequestType::RRT_GET, "/testclient?id=abc", "1.0");
        std::vector<char> returnVector = client.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>());
        REQUIRE(returnVector != std::vector<char>());
        std::string returnString(&returnVector[0], returnVector.size());
        REQUIRE(returnString.find("HTTP/1.0 400 Bad Request") != std::string::npos);
    }
    SECTION("Valid Type: GET with a valid id") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        nlohmann::json returnJson;
        returnJson["result"] = "Ok";
        mocks.NeverCall(restState, RestState::getClientJson);
        mocks.ExpectCall(restState, RestState::getClientJsonForId).With(1L).Return(returnJson);
        
        RestClient client("/testclient", *restState);
        RestRequest request(RestRequestType::RRT_GET, "/testclient?id=1", "1.0");
        std::vector<char> returnVector = client.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>());
        REQUIRE(returnVector != std::vector<char>());
        std::string returnString(&returnVector[0], returnVector.size());
        REQUIRE(returnString.find("HTTP/1.1 200 OK\r\n") != std::string::npos);

    }
    SECTION("Valid Type: DELETE with invalid id") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::stopClient);
        
        RestClient client("/testclient", *restState);
        RestRequest request(RestRequestType::RRT_DELETE, "/testclient?id=abc", "1.0");
        std::vector<char> returnVector = client.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>());
        REQUIRE(returnVector != std::vector<char>());
        std::string returnString(&returnVector[0], returnVector.size());
        REQUIRE(returnString.find("HTTP/1.0 400 Bad Request") != std::string::npos);
    }
    SECTION("Valid Type: DELETE with no id") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::stopClient);
        
        RestClient client("/testclient", *restState);
        RestRequest request(RestRequestType::RRT_DELETE, "/testclient", "1.0");
        std::vector<char> returnVector = client.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>());
        REQUIRE(returnVector != std::vector<char>());
        std::string returnString(&returnVector[0], returnVector.size());
        REQUIRE(returnString.find("HTTP/1.0 400 Bad Request") != std::string::npos);
    }
    SECTION("Valid Type: DELETE with valid id") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getClientJson);
        mocks.ExpectCall(restState, RestState::stopClient).With(1).Return(true);
        
        RestClient client("/testclient", *restState);
        RestRequest request(RestRequestType::RRT_DELETE, "/testclient?id=1", "1.0");
        std::vector<char> returnVector = client.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>());
        REQUIRE(returnVector != std::vector<char>());
        std::string returnString(&returnVector[0], returnVector.size());
        REQUIRE(returnString.find("HTTP/1.1 200 OK\r\n") != std::string::npos);
        REQUIRE(returnString.find("\"result\":\"Ok\"") != std::string::npos);
    }
    SECTION("Valid Type: DELETE with valid id but stopClient fails") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getClientJson);
        mocks.ExpectCall(restState, RestState::stopClient).With(1).Return(false);
        
        RestClient client("/testclient", *restState);
        RestRequest request(RestRequestType::RRT_DELETE, "/testclient?id=1", "1.0");
        std::vector<char> returnVector = client.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>());
        REQUIRE(returnVector != std::vector<char>());
        std::string returnString(&returnVector[0], returnVector.size());
        REQUIRE(returnString.find("HTTP/1.1 200 OK\r\n") != std::string::npos);
        REQUIRE(returnString.find("\"result\":\"Failed\"") != std::string::npos);
    }
    SECTION("Valid Type: POST with no parameters") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::startClient);
        
        RestClient client("/testclient", *restState);
        RestRequest request(RestRequestType::RRT_PUT, "/testclient", "1.0");
        std::vector<char> returnVector = client.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>());
        REQUIRE(returnVector != std::vector<char>());
        std::string returnString(&returnVector[0], returnVector.size());
        REQUIRE(returnString.find("HTTP/1.0 400 Bad Request") != std::string::npos);
    }
    SECTION("Valid Type: POST with only host") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::startClient);
        
        RestClient client("/testclient", *restState);
        RestRequest request(RestRequestType::RRT_PUT, "/testclient?host=127.0.0.1", "1.0");
        std::vector<char> returnVector = client.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>());
        REQUIRE(returnVector != std::vector<char>());
        std::string returnString(&returnVector[0], returnVector.size());
        REQUIRE(returnString.find("HTTP/1.0 400 Bad Request") != std::string::npos);
    }
    SECTION("Valid Type: POST with host but invalid port") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::startClient);
        
        RestClient client("/testclient", *restState);
        RestRequest request(RestRequestType::RRT_PUT, "/testclient?host=127.0.0.1&port=a", "1.0");
        std::vector<char> returnVector = client.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>());
        REQUIRE(returnVector != std::vector<char>());
        std::string returnString(&returnVector[0], returnVector.size());
        REQUIRE(returnString.find("HTTP/1.0 400 Bad Request") != std::string::npos);
    }
    SECTION("Valid Type: POST with host and valid port and succeeds") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getClientJson);
        mocks.ExpectCall(restState, RestState::startClient).Return(true);
        
        RestClient client("/testclient", *restState);
        RestRequest request(RestRequestType::RRT_PUT, "/testclient?host=127.0.0.1&port=80", "1.0");
        std::vector<char> returnVector = client.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>());
        REQUIRE(returnVector != std::vector<char>());
        std::string returnString(&returnVector[0], returnVector.size());
        REQUIRE(returnString.find("HTTP/1.1 200 OK\r\n") != std::string::npos);
        REQUIRE(returnString.find("\"result\":\"Ok\"") != std::string::npos);
    }
    SECTION("Valid Type: POST with host and valid port and fails") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getClientJson);
        mocks.ExpectCall(restState, RestState::startClient).Return(false);
        
        RestClient client("/testclient", *restState);
        RestRequest request(RestRequestType::RRT_PUT, "/testclient?host=127.0.0.1&port=80", "1.0");
        std::vector<char> returnVector = client.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>());
        REQUIRE(returnVector != std::vector<char>());
        std::string returnString(&returnVector[0], returnVector.size());
        REQUIRE(returnString.find("HTTP/1.1 200 OK\r\n") != std::string::npos);
        REQUIRE(returnString.find("\"result\":\"Failed\"") != std::string::npos);
    }
    SECTION("Valid Type: POST with all parameters") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getClientJson);
        mocks.ExpectCall(restState, RestState::startClient).Return(true);
        
        RestClient client("/testclient", *restState);
        RestRequest request(RestRequestType::RRT_PUT, "/testclient?host=127.0.0.1&port=80&id=1&count=1&min=1&max=10&contentmanager=fixed", "1.0");
        std::vector<char> returnVector = client.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>());
        REQUIRE(returnVector != std::vector<char>());
        std::string returnString(&returnVector[0], returnVector.size());
        REQUIRE(returnString.find("HTTP/1.1 200 OK\r\n") != std::string::npos);
        REQUIRE(returnString.find("\"result\":\"Ok\"") != std::string::npos);
    }
    SECTION("Valid Type: POST with all parameters, invalid id") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::startClient);
        
        RestClient client("/testclient", *restState);
        RestRequest request(RestRequestType::RRT_PUT, "/testclient?host=127.0.0.1&port=80&id=a&count=1&min=1&max=10&contentmanager=fixed", "1.0");
        std::vector<char> returnVector = client.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>());
        REQUIRE(returnVector != std::vector<char>());
        std::string returnString(&returnVector[0], returnVector.size());
        REQUIRE(returnString.find("HTTP/1.0 400 Bad Request") != std::string::npos);
    }
    SECTION("Valid Type: POST with all parameters, invalid count") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::startClient);
        
        RestClient client("/testclient", *restState);
        RestRequest request(RestRequestType::RRT_PUT, "/testclient?host=127.0.0.1&port=80&id=1&count=a&min=1&max=10&contentmanager=fixed", "1.0");
        std::vector<char> returnVector = client.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>());
        REQUIRE(returnVector != std::vector<char>());
        std::string returnString(&returnVector[0], returnVector.size());
        REQUIRE(returnString.find("HTTP/1.0 400 Bad Request") != std::string::npos);
    }
    SECTION("Valid Type: POST with all parameters, invalid min") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::startClient);
        
        RestClient client("/testclient", *restState);
        RestRequest request(RestRequestType::RRT_PUT, "/testclient?host=127.0.0.1&port=80&id=1&count=1&min=a&max=10&contentmanager=fixed", "1.0");
        std::vector<char> returnVector = client.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>());
        REQUIRE(returnVector != std::vector<char>());
        std::string returnString(&returnVector[0], returnVector.size());
        REQUIRE(returnString.find("HTTP/1.0 400 Bad Request") != std::string::npos);
    }
    SECTION("Valid Type: POST with all parameters, invalid max") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::startClient);
        
        RestClient client("/testclient", *restState);
        RestRequest request(RestRequestType::RRT_PUT, "/testclient?host=127.0.0.1&port=80&id=1&count=1&min=1&max=a&contentmanager=fixed", "1.0");
        std::vector<char> returnVector = client.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>());
        REQUIRE(returnVector != std::vector<char>());
        std::string returnString(&returnVector[0], returnVector.size());
        REQUIRE(returnString.find("HTTP/1.0 400 Bad Request") != std::string::npos);
    }
    SECTION("Valid Type: POST with all parameters, invalid content_manager") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::startClient);
        
        RestClient client("/testclient", *restState);
        RestRequest request(RestRequestType::RRT_PUT, "/testclient?host=127.0.0.1&port=80&id=1&count=1&min=1&max=10&contentmanager=boo", "1.0");
        std::vector<char> returnVector = client.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>());
        REQUIRE(returnVector != std::vector<char>());
        std::string returnString(&returnVector[0], returnVector.size());
        REQUIRE(returnString.find("HTTP/1.0 400 Bad Request") != std::string::npos);
    }
}
