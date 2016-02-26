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
#include "rest/html_return.h"


TEST_CASE("returnHtmlPage", "[rest][content][server]") {
    SECTION("returnHtmlPage: Content") {
        const std::string expectedString = "HTTP/1.0 999 uNiQue\r\nConnection: close\r\nContent-Length: 124\r\n\r\n<!DOCTYPE HTML PUBLIC ""-//IETF//DTD HTML 2.0//EN"">\r\n<html><head><title>TiTle</title></head><body><h1>BoDy</h1></body></html>\r\n";
        std::vector<char> testValue = returnHtmlPage(999, "uNiQue", "TiTle", "BoDy");
        std::string testString(&testValue[0], testValue.size());
        REQUIRE(testString.find(expectedString) != std::string::npos);
    }
}

TEST_CASE("returnJsonPage", "[rest][content][server]") {
    SECTION("returnJsonPage: Content") {
        std::vector<char> testValue = returnJsonPage(999, "uNiQue", "DuMp");
        std::string testString(&testValue[0], testValue.size());
        REQUIRE(testString.find("HTTP/1.1 999 uNiQue\r\nConnection: open\r\nContent-Length: 4\r\n\r\nDuMp") != std::string::npos);
    }
}