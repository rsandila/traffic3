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

#include <cstdio>
#include "catch.hpp"
#include "rest/static_rest_request_handler.cpp"

TEST_CASE("StaticRestRequestHandler", "[rest][server][content]") {
    class TemporaryTestFile {
    public:
        TemporaryTestFile(const std::string & fileName, const std::string & testContent) : path(fileName) {
            std::ofstream outp(fileName);
            if (outp) {
                outp << testContent;
            }
        }
        virtual ~TemporaryTestFile() {
            unlink(path.c_str());
        }
        private:
            const std::string path;
    };
    SECTION("Test default") {
        TemporaryTestFile defaultTestFile("testfile_default.html", "test content");
        StaticRestRequestHandler handler(".", "testfile_default.html", "/test/(.*)");
        
        RestRequest request(RestRequestType::RRT_GET, "/test/", "1.0");
        std::vector<char> returnVector = handler.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>());
        std::string returnString(&returnVector[0], returnVector.size());
        
        REQUIRE(returnString.find("test content") != std::string::npos);
        REQUIRE(returnString.find("HTTP/1.0 200 OK") != std::string::npos);
    }
    SECTION("Test file that does not exist") {
        StaticRestRequestHandler handler(".", "testfile_default.html", "/test/(.*)");
        
        RestRequest request(RestRequestType::RRT_GET, "/test/bla.html", "1.0");
        REQUIRE(handler.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>()) == std::vector<char>());
    }
    SECTION("Test file that exists") {
        TemporaryTestFile testFile("testfile.html", "test content2");
        StaticRestRequestHandler handler(".", "testfile_default.html", "/test/(.*)");
        
        RestRequest request(RestRequestType::RRT_GET, "/test/testfile.html", "1.0");
        std::vector<char> returnVector = handler.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>());
        std::string returnString(&returnVector[0], returnVector.size());
        
        REQUIRE(returnString.find("test content2") != std::string::npos);
        REQUIRE(returnString.find("HTTP/1.0 200 OK") != std::string::npos);
    }
    SECTION("PUT") {
        TemporaryTestFile defaultTestFile("testfile_default.html", "test content");
        StaticRestRequestHandler handler(".", "testfile_default.html", "/test/(.*)");
        
        RestRequest request(RestRequestType::RRT_PUT, "/test/testfile_default.html", "1.0");
        REQUIRE(handler.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>()) == std::vector<char>());
    }
    SECTION("POST") {
        TemporaryTestFile defaultTestFile("testfile_default.html", "test content");
        StaticRestRequestHandler handler(".", "testfile_default.html", "/test/(.*)");
        
        RestRequest request(RestRequestType::RRT_POST, "/test/testfile_default.html", "1.0");
        REQUIRE(handler.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>()) == std::vector<char>());
    }
    SECTION("HEAD") {
        TemporaryTestFile defaultTestFile("testfile_default.html", "test content");
        StaticRestRequestHandler handler(".", "testfile_default.html", "/test/(.*)");
        
        RestRequest request(RestRequestType::RRT_HEAD, "/test/testfile_default.html", "1.0");
        REQUIRE(handler.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>()) == std::vector<char>());
    }
    SECTION("DELETE") {
        TemporaryTestFile defaultTestFile("testfile_default.html", "test content");
        StaticRestRequestHandler handler(".", "testfile_default.html", "/test/(.*)");
        
        RestRequest request(RestRequestType::RRT_DELETE, "/test/testfile_default.html", "1.0");
        REQUIRE(handler.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>()) == std::vector<char>());
    }
    SECTION("TRACE") {
        TemporaryTestFile defaultTestFile("testfile_default.html", "test content");
        StaticRestRequestHandler handler(".", "testfile_default.html", "/test/(.*)");
        
        RestRequest request(RestRequestType::RRT_TRACE, "/test/testfile_default.html", "1.0");
        REQUIRE(handler.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>()) == std::vector<char>());
    }
    SECTION("OPTIONS") {
        TemporaryTestFile defaultTestFile("testfile_default.html", "test content");
        StaticRestRequestHandler handler(".", "testfile_default.html", "/test/(.*)");
        
        RestRequest request(RestRequestType::RRT_OPTIONS, "/test/testfile_default.html", "1.0");
        REQUIRE(handler.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>()) == std::vector<char>());
    }
    SECTION("CONNECT") {
        TemporaryTestFile defaultTestFile("testfile_default.html", "test content");
        StaticRestRequestHandler handler(".", "testfile_default.html", "/test/(.*)");
        
        RestRequest request(RestRequestType::RRT_CONNECT, "/test/testfile_default.html", "1.0");
        REQUIRE(handler.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(), std::vector<char>()) == std::vector<char>());
    }
}