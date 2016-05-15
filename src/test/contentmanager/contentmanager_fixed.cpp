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
// Testing contentmanager_fixed
#include <memory.h>
#include "catch.hpp"
#include "contentmanager/contentmanager_fixed.h"
#include "lib/common.h"

static std::vector<char> fixedLastWrite;
static std::atomic<int> fixedOrder(0);
static std::atomic<int> fixedReadOrder(0);
static std::atomic<int> fixedWriteOrder(0);
static std::atomic<bool> fixedDoExit(false);
static std::vector<char> compareTo(20, 'A');

static void fixedAssignOrder(std::atomic<int> & variable) {
    ++fixedOrder;
    if (variable == 0) {
        variable = (int)fixedOrder;
    }
}

class FixedMockProtocol: public Protocol {
public:
    FixedMockProtocol() : Protocol("Mock") {
    }
    virtual bool read(std::vector<char> & data,  bool allowPartialRead, Host & hostState) override {
        UNUSED(allowPartialRead);
        UNUSED(hostState);
        if (fixedDoExit) {
            return false;
        }
        fixedAssignOrder(fixedReadOrder);
        REQUIRE(data.size() == 8);
        memcpy(&data[0], "TRAF", 4);
        uint32_t size = htonl(8);
        memcpy(&data[4], &size, sizeof(uint32_t));
        fixedDoExit = true;
        updateBytesRead(data.size());
        return true;
    };
    virtual void close() override {
        fixedDoExit = true;
    }

    virtual bool write(const std::vector<char> & data, const Host & hostState) override {
        UNUSED(hostState);
        fixedLastWrite = data;
        fixedAssignOrder(fixedWriteOrder);
        updateBytesWritten(data.size());
        return true;
    }
};

TEST_CASE("Server: Test generating fixed buffer", "[content][server]") {
    SECTION("Test text generated") {
        std::unique_ptr<Protocol> proto(new FixedMockProtocol());
        fixedDoExit = false;
        fixedOrder = 0;
        fixedReadOrder = 0;
        fixedWriteOrder = 0;
        CommonHeaders commonHeaders;
        ContentManager_Fixed manager(std::move(proto), commonHeaders, true);
        manager.setMinimumSize('A');
        manager.setMaximumSize(20);
        REQUIRE(manager.Start());
        while (fixedLastWrite.size() == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        REQUIRE(fixedLastWrite.size() == 20);
        REQUIRE(fixedLastWrite == compareTo);
        REQUIRE(fixedReadOrder < fixedWriteOrder);
        
        REQUIRE(8 == manager.getBytesRead());
        REQUIRE(28 == manager.getBytesWritten()); // 20 + 8
        
        nlohmann::json json = manager.toJson();
        REQUIRE(json.size() == 7);
        REQUIRE(json["min"].get<unsigned>() == 'A');
        REQUIRE(json["max"].get<unsigned>() == 20);
        REQUIRE(json["started"].get<bool>() == true);
        REQUIRE(json["running"].get<bool>() == true);
        REQUIRE(json["exitBeforeState"].get<bool>() == false);
        REQUIRE(json["protocol"].is_object() == true);
        REQUIRE(json["commonHeaders"].is_object() == true);
    }
}

TEST_CASE("Client: Test generating fixed buffer", "[content][client]") {
    SECTION("Test text generated") {
        fixedDoExit = false;
        fixedOrder = 0;
        fixedReadOrder = 0;
        fixedWriteOrder = 0;
        std::unique_ptr<Protocol> proto(new FixedMockProtocol());
        CommonHeaders commonHeaders;
        ContentManager_Fixed manager(std::move(proto), commonHeaders, false);
        manager.setMinimumSize('A');
        manager.setMaximumSize(20);
        REQUIRE(manager.Start());
        while (fixedDoExit == false) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        REQUIRE(fixedLastWrite.size() == 20);
        REQUIRE(fixedLastWrite == compareTo);
        REQUIRE(fixedReadOrder > fixedWriteOrder);
        
        REQUIRE(8 == manager.getBytesRead());
        REQUIRE(56 == manager.getBytesWritten()); // 2 * (20 + 8)
        
        nlohmann::json json = manager.toJson();
        REQUIRE(json.size() == 7);
        REQUIRE(json["min"].get<unsigned>() == 'A');
        REQUIRE(json["max"].get<unsigned>() == 20);
        REQUIRE(json["started"].get<bool>() == true);
        REQUIRE(json["running"].get<bool>() == true);
        REQUIRE(json["exitBeforeState"].get<bool>() == false);
        REQUIRE(json["protocol"].is_object() == true);
        REQUIRE(json["commonHeaders"].is_object() == true);
    }
}


