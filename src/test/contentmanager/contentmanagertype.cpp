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
#include "contentmanager/contentmanagertype.h"

TEST_CASE("Testing ContentManagerType: convertStringToContentManagerType", "[content]") {
    SECTION("randomtext") {
        REQUIRE(convertStringToContentManagerType("randomtext") == ContentManagerType::RandomText);
    }
    SECTION("randombinary") {
        REQUIRE(convertStringToContentManagerType("randombinary") == ContentManagerType::RandomBinary);
    }
    SECTION("fixed") {
        REQUIRE(convertStringToContentManagerType("fixed") == ContentManagerType::Fixed);
    }
    SECTION("echo") {
        REQUIRE(convertStringToContentManagerType("echo") == ContentManagerType::Echo);
    }
    SECTION("unknown") {
        REQUIRE_THROWS_AS(convertStringToContentManagerType("unknown"), std::invalid_argument);
    }
}

TEST_CASE("Testing ContentManagerType: convertContentManagerTypeToString", "[content]") {
    SECTION("randomtext") {
        REQUIRE("randomtext" == convertContentManagerTypeToString(ContentManagerType::RandomText));
    }
    SECTION("randombinary") {
        REQUIRE("randombinary" == convertContentManagerTypeToString(ContentManagerType::RandomBinary));
    }
    SECTION("fixed") {
        REQUIRE("fixed" == convertContentManagerTypeToString(ContentManagerType::Fixed));
    }
    SECTION("echo") {
        REQUIRE("echo" == convertContentManagerTypeToString(ContentManagerType::Echo));
    }
}
