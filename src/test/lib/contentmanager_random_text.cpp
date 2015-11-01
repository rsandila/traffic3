// Testing contentmanager_random_text
#include <thread>
#include "catch.hpp"
#include "protocol.h"
#include "contentmanager_random_text.h"

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
    MockProtocol() {
    }
    virtual bool read(std::vector<char> & data, bool allowPartialRead) override {
        if (doExit) {
            return false;
        }
        assignOrder(readOrder);
        REQUIRE(data.size() == 8);
        memcpy(&data[0], "TRAF", 4);
        uint32_t size = htonl(8);
        memcpy(&data[4], &size, sizeof(uint32_t));
        doExit = true;
        return true;
    };
    virtual void close() override {
        doExit = true;
    }

    virtual bool write(const std::vector<char> & data) override {
        lastWrite = data;
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
        CommonHeaders commonHeaders;
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
    }
}

TEST_CASE("Client: Test random generating random text", "[content][client]") {
    SECTION("Test text generated") {
        doExit = false;
        order = 0;
        readOrder = 0;
        writeOrder = 0;
        std::unique_ptr<Protocol> proto(new MockProtocol());
        CommonHeaders commonHeaders;
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
    }
}
