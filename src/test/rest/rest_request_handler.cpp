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
#include "rest/rest_request_handler.h"

TEST_CASE("RestRequestHandler", "[rest]") {
    SECTION("Returns empty vector") {
        RestRequestHandler handler;
        
        RestRequest request(RestRequestType::RRT_GET, "/testclient?boo=123", "1.0");
        
        REQUIRE(handler.handleRequest(Host::ALL_INTERFACES4, request, std::map<std::string, std::string>(),
                                        std::vector<char>()) == std::vector<char>());
    }
}
