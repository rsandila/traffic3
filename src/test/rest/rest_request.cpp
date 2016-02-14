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

#include "catch.hpp"
#include "rest/rest_request.h"

/*
 bool hasParam(const std::string & name) const noexcept;
 */

TEST_CASE("RestRequest: constructor", "[request][rest]") {
    SECTION("Missing value") {
        try {
            RestRequest request(RestRequestType::RRT_CONNECT, "/dummy?id=10&bla=", "1.0");
            FAIL("Expect invalid_argument, should never get here");
        } catch (std::invalid_argument &e) {
            // success
        }
    }
}

TEST_CASE("RestRequest: getType", "[request][rest]") {
    SECTION("GET") {
        RestRequest request(RestRequestType::RRT_GET, "/", "1.0");
        REQUIRE(RestRequestType::RRT_GET == request.getType());
    }
    SECTION("PUT") {
        RestRequest request(RestRequestType::RRT_PUT, "/", "1.0");
        REQUIRE(RestRequestType::RRT_PUT == request.getType());
    }
    SECTION("POST") {
        RestRequest request(RestRequestType::RRT_POST, "/", "1.0");
        REQUIRE(RestRequestType::RRT_POST == request.getType());
    }
    SECTION("HEAD") {
        RestRequest request(RestRequestType::RRT_HEAD, "/", "1.0");
        REQUIRE(RestRequestType::RRT_HEAD == request.getType());
    }
    SECTION("DELETE") {
        RestRequest request(RestRequestType::RRT_DELETE, "/", "1.0");
        REQUIRE(RestRequestType::RRT_DELETE == request.getType());
    }
    SECTION("TRACE") {
        RestRequest request(RestRequestType::RRT_TRACE, "/", "1.0");
        REQUIRE(RestRequestType::RRT_TRACE == request.getType());
    }
    SECTION("OPTIONS") {
        RestRequest request(RestRequestType::RRT_OPTIONS, "/", "1.0");
        REQUIRE(RestRequestType::RRT_OPTIONS == request.getType());
    }
    SECTION("CONNECT") {
        RestRequest request(RestRequestType::RRT_CONNECT, "/", "1.0");
        REQUIRE(RestRequestType::RRT_CONNECT == request.getType());
    }
}

TEST_CASE("RestRequest: getUri", "[request][rest]") {
    SECTION("No parameters") {
        RestRequest request(RestRequestType::RRT_CONNECT, "/dummy", "1.0");
        REQUIRE(std::string("/dummy") == request.getUri());
    }
    SECTION("One parameter") {
        RestRequest request(RestRequestType::RRT_CONNECT, "/dummy?test=1", "1.0");
        REQUIRE(std::string("/dummy") == request.getUri());
    }
    SECTION("Multiple parameters") {
        RestRequest request(RestRequestType::RRT_CONNECT, "/dummy?test=1&bla=boo", "1.0");
        REQUIRE(std::string("/dummy") == request.getUri());
    }
}

TEST_CASE("RestRequest: getVersion", "[request][rest]") {
    SECTION("Valid") {
        RestRequest request(RestRequestType::RRT_CONNECT, "/dummy", "1.0");
        REQUIRE(std::string("1.0") == request.getVersion());
    }
}

TEST_CASE("RestRequest: getParam", "[request][rest]") {
    SECTION("No parameter") {
        RestRequest request(RestRequestType::RRT_CONNECT, "/dummy", "1.0");
        REQUIRE(std::string("") == request.getParam("id"));
    }
    SECTION("Other parameter") {
        RestRequest request(RestRequestType::RRT_CONNECT, "/dummy?bla=boo", "1.0");
        REQUIRE(std::string("") == request.getParam("id"));
    }
    SECTION("Only the right parameter") {
        RestRequest request(RestRequestType::RRT_CONNECT, "/dummy?id=1", "1.0");
        REQUIRE(std::string("1") == request.getParam("id"));
    }
    SECTION("Multiple parameters") {
        RestRequest request(RestRequestType::RRT_CONNECT, "/dummy?id=10&bla=boo", "1.0");
        REQUIRE(std::string("10") == request.getParam("id"));
        REQUIRE(std::string("boo") == request.getParam("bla"));
    }
}

TEST_CASE("RestRequest: getParamWithDefault", "[request][rest]") {
    SECTION("No parameter") {
        RestRequest request(RestRequestType::RRT_CONNECT, "/dummy", "1.0");
        REQUIRE(std::string("0") == request.getParamWithDefault("id", "0"));
    }
    SECTION("Other parameter") {
        RestRequest request(RestRequestType::RRT_CONNECT, "/dummy?bla=boo", "1.0");
        REQUIRE(std::string("0") == request.getParamWithDefault("id", "0"));
    }
    SECTION("Only the right parameter") {
        RestRequest request(RestRequestType::RRT_CONNECT, "/dummy?id=1", "1.0");
        REQUIRE(std::string("1") == request.getParamWithDefault("id", "0"));
    }
    SECTION("Multiple parameters") {
        RestRequest request(RestRequestType::RRT_CONNECT, "/dummy?id=10&bla=boo", "1.0");
        REQUIRE(std::string("10") == request.getParamWithDefault("id", "0"));
        REQUIRE(std::string("boo") == request.getParamWithDefault("bla", "0"));
    }
}

TEST_CASE("RestRequest: hasParam", "[request][rest]") {
    SECTION("No parameter") {
        RestRequest request(RestRequestType::RRT_CONNECT, "/dummy", "1.0");
        REQUIRE_FALSE(request.hasParam("id"));
    }
    SECTION("Has single parameter") {
        RestRequest request(RestRequestType::RRT_CONNECT, "/dummy?id=1", "1.0");
        REQUIRE(request.hasParam("id"));
    }
    SECTION("Has multiple parameters") {
        RestRequest request(RestRequestType::RRT_CONNECT, "/dummy?id=1&dummy=bla", "1.0");
        REQUIRE(request.hasParam("id"));
        REQUIRE(request.hasParam("dummy"));
    }
}
