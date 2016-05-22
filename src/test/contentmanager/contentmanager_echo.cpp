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
// Testing contentmanager_echo
#include <memory.h>
#include "catch.hpp"
#include "contentmanager/contentmanager_echo.h"
#include "lib/common.h"

static std::vector<char> echoLastWrite;
static std::atomic<int> echoOrder(0);
static std::atomic<int> echoReadOrder(0);
static std::atomic<int> echoWriteOrder(0);
static std::atomic<bool> echoDoExit(false);
static std::vector<char> compareTo(20, 'A');

static void echoAssignOrder(std::atomic<int> & variable) {
    ++echoOrder;
    if (variable == 0) {
        variable = (int)echoOrder;
    }
}

class EchoMockProtocol: public Protocol {
public:
    EchoMockProtocol() : Protocol("Mock"), firstRead(false) {
    }
    virtual bool read(std::vector<char> & data,  bool allowPartialRead, Host & hostState) override {
        UNUSED(allowPartialRead);
        UNUSED(hostState);
        if (echoDoExit) {
            return false;
        }
        if (!firstRead) {
            echoAssignOrder(echoReadOrder);
            REQUIRE(data.size() == 8);
            memcpy(&data[0], "TRAF", 4);
            uint32_t size = htonl(8 + compareTo.size());
            memcpy(&data[4], &size, sizeof(uint32_t));
            updateBytesRead(data.size());
            firstRead = true;
        } else {
            REQUIRE(data.size() == compareTo.size());
            data = compareTo;
            updateBytesRead(data.size());
            echoDoExit = true;
        }
        return true;
    };
    virtual void close() override {
        echoDoExit = true;
    }
    
    virtual bool write(const std::vector<char> & data, const Host & hostState) override {
        UNUSED(hostState);
        echoLastWrite = data;
        updateBytesWritten(data.size());
        echoAssignOrder(echoWriteOrder);
        return true;
    }
    bool firstRead;
};

TEST_CASE("Server: Test generating echo buffer", "[content][server]") {
    SECTION("Test text generated") {
        std::unique_ptr<Protocol> proto(new EchoMockProtocol());
        echoDoExit = false;
        echoOrder = 0;
        echoReadOrder = 0;
        echoWriteOrder = 0;
        std::shared_ptr<CommonHeaders> commonHeaders(new CommonHeaders());
        ContentManager_Echo manager(std::move(proto), commonHeaders, true);
        REQUIRE(manager.Start());
        while (echoLastWrite.size() == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        REQUIRE(echoLastWrite.size() == 20);
        REQUIRE(echoLastWrite == compareTo);
        REQUIRE(echoReadOrder < echoWriteOrder);
        REQUIRE(28 == manager.getBytesRead());
        REQUIRE(28 == manager.getBytesWritten());
        
        nlohmann::json json = manager.toJson();
        REQUIRE(json.size() == 7);
        REQUIRE(json["min"].get<unsigned>() >= 0);
        REQUIRE(json["max"].get<unsigned>() >= 0);
        REQUIRE(json["started"].get<bool>() == true);
        REQUIRE(json["running"].get<bool>() == true);
        REQUIRE(json["exitBeforeState"].get<bool>() == false);
        REQUIRE(json["protocol"].is_object() == true);
        REQUIRE(json["commonHeaders"].is_object() == true);
    }
}

TEST_CASE("Client: Test generating echo buffer", "[content][client]") {
    SECTION("Test text generated") {
        echoDoExit = false;
        echoOrder = 0;
        echoReadOrder = 0;
        echoWriteOrder = 0;
        std::unique_ptr<Protocol> proto(new EchoMockProtocol());
        std::shared_ptr<CommonHeaders> commonHeaders(new CommonHeaders());
        ContentManager_Echo manager(std::move(proto), commonHeaders, false);
        REQUIRE(manager.Start());
        while (echoDoExit == false) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        REQUIRE(echoLastWrite.size() == 20);
        REQUIRE(echoLastWrite == compareTo);
        REQUIRE(echoReadOrder > echoWriteOrder);
        REQUIRE(28 == manager.getBytesRead());
        REQUIRE(36 == manager.getBytesWritten());

        nlohmann::json json = manager.toJson();
        REQUIRE(json.size() == 7);
        REQUIRE(json["min"].get<unsigned>() >= 0);
        REQUIRE(json["max"].get<unsigned>() >= 0);
        REQUIRE(json["started"].get<bool>() == true);
        REQUIRE(json["running"].get<bool>() == true);
        REQUIRE(json["exitBeforeState"].get<bool>() == false);
        REQUIRE(json["protocol"].is_object() == true);
        REQUIRE(json["commonHeaders"].is_object() == true);
    }
}
