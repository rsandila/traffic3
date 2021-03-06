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
// Testing contentmanager_random_text
#include <memory.h>
#include <thread>
#include "catch.hpp"
#include "protocol/protocol.h"
#include "contentmanager/contentmanager_random_text.h"
#include "lib/common.h"

static std::vector<char> lastWrite;
static std::atomic<int> order(0);
static std::atomic<int> readOrder(0);
static std::atomic<int> writeOrder(0);
static std::atomic<bool> doExit(false);

static void assignOrder(std::atomic<int> & variable) {
    ++order;
    if (variable == 0) {
        variable = (int)order;
    }
}

class MockProtocol: public Protocol {
public:
    MockProtocol() : Protocol("Mock") {
    }
    virtual bool read(std::vector<char> & data, bool allowPartialRead, Host & hostState) override {
        UNUSED(allowPartialRead);
        UNUSED(hostState);
        if (doExit) {
            return false;
        }
        assignOrder(readOrder);
        REQUIRE(data.size() == 8);
        memcpy(&data[0], "TRAF", 4);
        uint32_t size = htonl(8);
        memcpy(&data[4], &size, sizeof(uint32_t));
        doExit = true;
        updateBytesRead(data.size());
        return true;
    };
    virtual void close() override {
        doExit = true;
    }

    virtual bool write(const std::vector<char> & data, const Host & hostState) override {
        UNUSED(hostState);
        lastWrite = data;
        updateBytesWritten(data.size());
        assignOrder(writeOrder);
        return true;
    }
};

TEST_CASE("Server: Test random generating random text", "[content][server]") {
    SECTION("Test text generated") {
        std::unique_ptr<Protocol> proto(new MockProtocol());
        doExit = false;
        order = 0;
        readOrder = 0;
        writeOrder = 0;
        std::shared_ptr<CommonHeaders> commonHeaders(new CommonHeaders());
        ContentManager_Random_Text manager(std::move(proto), commonHeaders, true);
        manager.setMinimumSize(10);
        manager.setMaximumSize(20);
        REQUIRE(manager.Start());
        while (lastWrite.size() == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        REQUIRE(lastWrite.size() >= 10);
        REQUIRE(lastWrite.size() <= 20);
        REQUIRE(readOrder < writeOrder);
        
        REQUIRE(8 == manager.getBytesRead());
        REQUIRE(18 <= manager.getBytesWritten()); //  (10 + 8)
        REQUIRE(28 >= manager.getBytesWritten()); //  (20 + 8)
        
        nlohmann::json json = manager.toJson();
        REQUIRE(json.size() == 8);
        REQUIRE(json["min"].get<unsigned>() == 10);
        REQUIRE(json["max"].get<unsigned>() == 20);
        REQUIRE(json["started"].get<bool>() == true);
        REQUIRE(json["running"].get<bool>() == true);
        REQUIRE(json["exitBeforeState"].get<bool>() == false);
        REQUIRE(json["protocol"].is_object() == true);
        REQUIRE(json["commonHeaders"].is_object() == true);
    }
}

TEST_CASE("Client: Test random generating random text", "[content][client]") {
    SECTION("Test text generated") {
        doExit = false;
        order = 0;
        readOrder = 0;
        writeOrder = 0;
        std::unique_ptr<Protocol> proto(new MockProtocol());
        std::shared_ptr<CommonHeaders> commonHeaders(new CommonHeaders());
        ContentManager_Random_Text manager(std::move(proto), commonHeaders, false);
        manager.setMinimumSize(10);
        manager.setMaximumSize(20);
        REQUIRE(manager.Start());
        while (doExit == false) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        REQUIRE(lastWrite.size() >= 10);
        REQUIRE(lastWrite.size() <= 20);
        REQUIRE(readOrder > writeOrder);
        
        REQUIRE(8 == manager.getBytesRead());
        REQUIRE(36 <= manager.getBytesWritten()); // 2 * (10 + 8)
        REQUIRE(56 >= manager.getBytesWritten()); // 2 * (20 + 8)
        
        nlohmann::json json = manager.toJson();
        REQUIRE(json.size() == 8);
        REQUIRE(json["min"].get<unsigned>() == 10);
        REQUIRE(json["max"].get<unsigned>() == 20);
        REQUIRE(json["started"].get<bool>() == true);
        REQUIRE(json["running"].get<bool>() == true);
        REQUIRE(json["exitBeforeState"].get<bool>() == false);
        REQUIRE(json["protocol"].is_object() == true);
        REQUIRE(json["commonHeaders"].is_object() == true);
    }
}

