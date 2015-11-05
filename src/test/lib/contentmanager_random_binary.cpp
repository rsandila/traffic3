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
// Testing contentmanager_random_binary
#include <memory.h>
#include <thread>
#include "catch.hpp"
#include "protocol.h"
#include "contentmanager_random_binary.h"
#include "common.h"

static std::vector<char> randomBinaryLastWrite;
static std::atomic<int> randomBinaryOrder(0);
static std::atomic<int> randomBinaryReadOrder(0);
static std::atomic<int> randomBinaryWriteOrder(0);
static std::atomic<bool> randomBinaryDoExit(false);

static void assignOrder(std::atomic<int> & variable) {
    ++randomBinaryOrder;
    if (variable == 0) {
        variable = (int)randomBinaryOrder;
    }
}

class RandomBinaryMockProtocol: public Protocol {
public:
    RandomBinaryMockProtocol() {
    }
    virtual bool read(std::vector<char> & data, bool allowPartialRead) override {
        UNUSED(allowPartialRead);
        if (randomBinaryDoExit) {
            return false;
        }
        assignOrder(randomBinaryReadOrder);
        REQUIRE(data.size() == 8);
        memcpy(&data[0], "TRAF", 4);
        uint32_t size = htonl(8);
        memcpy(&data[4], &size, sizeof(uint32_t));
        randomBinaryDoExit = true;
        return true;
    };
    virtual void close() override {
        randomBinaryDoExit = true;
    }

    virtual bool write(const std::vector<char> & data) override {
        randomBinaryLastWrite = data;
        assignOrder(randomBinaryWriteOrder);
        return true;
    }
};

TEST_CASE("Server: Test random generating random binary", "[content][server]") {
    SECTION("Test binary generated") {
        std::unique_ptr<Protocol> proto(new RandomBinaryMockProtocol());
        randomBinaryDoExit = false;
        randomBinaryOrder = 0;
        randomBinaryReadOrder = 0;
        randomBinaryWriteOrder = 0;
        CommonHeaders commonHeaders;
        ContentManager_Random_Binary manager(std::move(proto), commonHeaders, true);
        manager.setMinimumSize(10);
        manager.setMaximumSize(20);
        REQUIRE(manager.Start());
        while (randomBinaryLastWrite.size() == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        REQUIRE(randomBinaryLastWrite.size() >= 10);
        REQUIRE(randomBinaryLastWrite.size() <= 20);
        REQUIRE(randomBinaryReadOrder < randomBinaryWriteOrder);
    }
}

TEST_CASE("Client: Test random generating random binary", "[content][client]") {
    SECTION("Test binary generated") {
        randomBinaryDoExit = false;
        randomBinaryOrder = 0;
        randomBinaryReadOrder = 0;
        randomBinaryWriteOrder = 0;
        std::unique_ptr<Protocol> proto(new RandomBinaryMockProtocol());
        CommonHeaders commonHeaders;
        ContentManager_Random_Binary manager(std::move(proto), commonHeaders, false);
        manager.setMinimumSize(10);
        manager.setMaximumSize(20);
        REQUIRE(manager.Start());
        while (randomBinaryDoExit == false) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        REQUIRE(randomBinaryLastWrite.size() >= 10);
        REQUIRE(randomBinaryLastWrite.size() <= 20);
        REQUIRE(randomBinaryReadOrder > randomBinaryWriteOrder);
    }
}
