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
    SECTION("Valid Type: GET unknown parameters and no id") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        nlohmann::json returnJson;
        returnJson["result"] = "Ok";
        
        mocks.ExpectCall(restState, RestState::getServerJson).Return(returnJson);
        
        RestServer server("/testserver", *restState);
        RestRequest request(RestRequestType::RRT_GET, "/testserver?boo=123", "1.0");
        
        REQUIRE(server.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>()) != std::vector<char>());
    }
    SECTION("Valid Type: GET with a valid id") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        nlohmann::json returnJson;
        returnJson["result"] = "Ok";
        mocks.NeverCall(restState, RestState::getServerJson);
        mocks.ExpectCall(restState, RestState::getServerJsonForId).With(1L).Return(returnJson);
        
        RestServer server("/testserver", *restState);
        RestRequest request(RestRequestType::RRT_GET, "/testserver?id=1", "1.0");
        std::vector<char> returnVector = server.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>());
        REQUIRE(returnVector != std::vector<char>());
        std::string returnString(&returnVector[0], returnVector.size());
        REQUIRE(returnString.find("HTTP/1.1 200 OK\r\n") != std::string::npos);
    }
    SECTION("Valid Type: GET id is not a number") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getServerJson);
        
        RestServer server("/testserver", *restState);
        RestRequest request(RestRequestType::RRT_GET, "/testserver?id=abc", "1.0");
        std::vector<char> returnVector = server.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>());
        REQUIRE(returnVector != std::vector<char>());
        std::string returnString(&returnVector[0], returnVector.size());
        REQUIRE(returnString.find("HTTP/1.0 400 Bad Request") != std::string::npos);
    }
    // TOOD - all the DELETE variants
    SECTION("Valid Type: DELETE with invalid id") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::stopServer);
        
        RestServer server("/testserver", *restState);
        RestRequest request(RestRequestType::RRT_DELETE, "/testserver?id=abc", "1.0");
        std::vector<char> returnVector = server.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>());
        REQUIRE(returnVector != std::vector<char>());
        std::string returnString(&returnVector[0], returnVector.size());
        REQUIRE(returnString.find("HTTP/1.0 400 Bad Request") != std::string::npos);
    }
    SECTION("Valid Type: DELETE with no id") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::stopServer);
        
        RestServer server("/testserver", *restState);
        RestRequest request(RestRequestType::RRT_DELETE, "/testserver", "1.0");
        std::vector<char> returnVector = server.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>());
        REQUIRE(returnVector != std::vector<char>());
        std::string returnString(&returnVector[0], returnVector.size());
        REQUIRE(returnString.find("HTTP/1.0 400 Bad Request") != std::string::npos);
    }
    SECTION("Valid Type: DELETE with valid id") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getServerJson);
        mocks.ExpectCall(restState, RestState::stopServer).With(1).Return(true);
        
        RestServer server("/testserver", *restState);
        RestRequest request(RestRequestType::RRT_DELETE, "/testserver?id=1", "1.0");
        std::vector<char> returnVector = server.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>());
        REQUIRE(returnVector != std::vector<char>());
        std::string returnString(&returnVector[0], returnVector.size());
        REQUIRE(returnString.find("HTTP/1.1 200 OK\r\n") != std::string::npos);
        REQUIRE(returnString.find("\"result\":\"Ok\"") != std::string::npos);
    }
    SECTION("Valid Type: DELETE with valid id but stopClient fails") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getServerJson);
        mocks.ExpectCall(restState, RestState::stopServer).With(1).Return(false);
        
        RestServer server("/testserver", *restState);
        RestRequest request(RestRequestType::RRT_DELETE, "/testserver?id=1", "1.0");
        std::vector<char> returnVector = server.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>());
        REQUIRE(returnVector != std::vector<char>());
        std::string returnString(&returnVector[0], returnVector.size());
        REQUIRE(returnString.find("HTTP/1.1 200 OK\r\n") != std::string::npos);
        REQUIRE(returnString.find("\"result\":\"Failed\"") != std::string::npos);
    }
    SECTION("Valid Type: PUT with no parameters") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::stopServer);
        
        RestServer server("/testserver", *restState);
        RestRequest request(RestRequestType::RRT_PUT, "/testserver", "1.0");
        std::vector<char> returnVector = server.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>());
        REQUIRE(returnVector != std::vector<char>());
        std::string returnString(&returnVector[0], returnVector.size());
        REQUIRE(returnString.find("HTTP/1.0 400 Bad Request") != std::string::npos);
    }
    SECTION("Valid Type: PUT with only port") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getServerJson);
        mocks.ExpectCall(restState, RestState::startServer).Return(true);
        
        RestServer server("/testserver", *restState);
        RestRequest request(RestRequestType::RRT_PUT, "/testserver?port=80", "1.0");
        std::vector<char> returnVector = server.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>());
        REQUIRE(returnVector != std::vector<char>());
        std::string returnString(&returnVector[0], returnVector.size());
        REQUIRE(returnString.find("HTTP/1.1 200 OK\r\n") != std::string::npos);
        REQUIRE(returnString.find("\"result\":\"Ok\"") != std::string::npos);
    }
    SECTION("Valid Type: PUT with all the parameters") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::getServerJson);
        mocks.ExpectCall(restState, RestState::startServer).Return(true);
        
        RestServer server("/testserver", *restState);
        RestRequest request(RestRequestType::RRT_PUT, "/testserver?port=80&id=0&protocol=tcp4&contentmanager=echo&host=127.0.0.1&max=0&min=0", "1.0");
        std::vector<char> returnVector = server.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>());
        REQUIRE(returnVector != std::vector<char>());
        std::string returnString(&returnVector[0], returnVector.size());
        REQUIRE(returnString.find("HTTP/1.1 200 OK\r\n") != std::string::npos);
        REQUIRE(returnString.find("\"result\":\"Ok\"") != std::string::npos);
    }
    SECTION("Valid Type: PUT with invalid id") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::stopServer);
        
        RestServer server("/testserver", *restState);
        RestRequest request(RestRequestType::RRT_PUT, "/testserver?port=80&id=a", "1.0");
        std::vector<char> returnVector = server.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>());
        REQUIRE(returnVector != std::vector<char>());
        std::string returnString(&returnVector[0], returnVector.size());
        REQUIRE(returnString.find("HTTP/1.0 400 Bad Request") != std::string::npos);
    }
    SECTION("Valid Type: PUT with invalid protocol") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::stopServer);
        
        RestServer server("/testserver", *restState);
        RestRequest request(RestRequestType::RRT_PUT, "/testserver?port=80&protocol=boo", "1.0");
        std::vector<char> returnVector = server.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>());
        REQUIRE(returnVector != std::vector<char>());
        std::string returnString(&returnVector[0], returnVector.size());
        REQUIRE(returnString.find("HTTP/1.0 400 Bad Request") != std::string::npos);
    }
    SECTION("Valid Type: PUT with invalid ContentManager") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::stopServer);
        
        RestServer server("/testserver", *restState);
        RestRequest request(RestRequestType::RRT_PUT, "/testserver?port=80&contentmanager=boo", "1.0");
        std::vector<char> returnVector = server.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>());
        REQUIRE(returnVector != std::vector<char>());
        std::string returnString(&returnVector[0], returnVector.size());
        REQUIRE(returnString.find("HTTP/1.0 400 Bad Request") != std::string::npos);
    }
    SECTION("Valid Type: PUT with invalid invalid port") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::stopServer);
        
        RestServer server("/testserver", *restState);
        RestRequest request(RestRequestType::RRT_PUT, "/testserver?port=port", "1.0");
        std::vector<char> returnVector = server.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>());
        REQUIRE(returnVector != std::vector<char>());
        std::string returnString(&returnVector[0], returnVector.size());
        REQUIRE(returnString.find("HTTP/1.0 400 Bad Request") != std::string::npos);
    }
    SECTION("Valid Type: PUT with invalid min") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::stopServer);
        
        RestServer server("/testserver", *restState);
        RestRequest request(RestRequestType::RRT_PUT, "/testserver?port=80&min=boo", "1.0");
        std::vector<char> returnVector = server.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>());
        REQUIRE(returnVector != std::vector<char>());
        std::string returnString(&returnVector[0], returnVector.size());
        REQUIRE(returnString.find("HTTP/1.0 400 Bad Request") != std::string::npos);
    }
    SECTION("Valid Type: PUT with invalid max") {
        MockRepository mocks;
        
        RestState * restState = mocks.Mock<RestState>();
        mocks.NeverCall(restState, RestState::stopServer);
        
        RestServer server("/testserver", *restState);
        RestRequest request(RestRequestType::RRT_PUT, "/testserver?port=80&max=boo", "1.0");
        std::vector<char> returnVector = server.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>());
        REQUIRE(returnVector != std::vector<char>());
        std::string returnString(&returnVector[0], returnVector.size());
        REQUIRE(returnString.find("HTTP/1.0 400 Bad Request") != std::string::npos);
    }
}
