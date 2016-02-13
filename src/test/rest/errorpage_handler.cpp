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
#include "rest/errorpage_handler.h"

TEST_CASE("ErrorPageHandler", "[rest][content][server]") {
    SECTION("EmptyRequest") {
        ErrorPageHandler errorPageHandler;
        std::vector<char> testValue = errorPageHandler.handleError(ErrorTypes::EmptyRequest, std::vector<char>());
        std::string testString(&testValue[0], testValue.size());
        REQUIRE(testString.find("HTTP/1.0 400") != testString.npos);
        REQUIRE(testString.find("\r\nConnection: close\r\nContent-Length:") != testString.npos);
        REQUIRE(testString.find("<html>") != std::string::npos);
        REQUIRE(testString.find("</html>") != std::string::npos);
    }
    SECTION("BadlyFormedRequest") {
        ErrorPageHandler errorPageHandler;
        std::vector<char> testValue = errorPageHandler.handleError(ErrorTypes::BadlyFormedRequest, std::vector<char>());
        std::string testString(&testValue[0], testValue.size());
        REQUIRE(testString.find("HTTP/1.0 400") != testString.npos);
        REQUIRE(testString.find("\r\nConnection: close\r\nContent-Length:") != testString.npos);
        REQUIRE(testString.find("<html>") != std::string::npos);
        REQUIRE(testString.find("</html>") != std::string::npos);
    }
    SECTION("UnknownRequestType") {
        ErrorPageHandler errorPageHandler;
        std::vector<char> testValue = errorPageHandler.handleError(ErrorTypes::UnknownRequestType, std::vector<char>());
        std::string testString(&testValue[0], testValue.size());
        REQUIRE(testString.find("HTTP/1.0 400") != testString.npos);
        REQUIRE(testString.find("\r\nConnection: close\r\nContent-Length:") != testString.npos);
        REQUIRE(testString.find("<html>") != std::string::npos);
        REQUIRE(testString.find("</html>") != std::string::npos);
    }
    SECTION("BadlyFormatedHeader") {
        ErrorPageHandler errorPageHandler;
        std::vector<char> testValue = errorPageHandler.handleError(ErrorTypes::BadlyFormatedHeader, std::vector<char>());
        std::string testString(&testValue[0], testValue.size());
        REQUIRE(testString.find("HTTP/1.0 400") != testString.npos);
        REQUIRE(testString.find("\r\nConnection: close\r\nContent-Length:") != testString.npos);
        REQUIRE(testString.find("<html>") != std::string::npos);
        REQUIRE(testString.find("</html>") != std::string::npos);
    }
    SECTION("NotFound") {
        ErrorPageHandler errorPageHandler;
        std::vector<char> testValue = errorPageHandler.handleError(ErrorTypes::NotFound, std::vector<char>());
        std::string testString(&testValue[0], testValue.size());
        REQUIRE(testString.find("HTTP/1.0 404") != testString.npos);
        REQUIRE(testString.find("\r\nConnection: close\r\nContent-Length:") != testString.npos);
        REQUIRE(testString.find("<html>") != std::string::npos);
        REQUIRE(testString.find("</html>") != std::string::npos);
    }
}