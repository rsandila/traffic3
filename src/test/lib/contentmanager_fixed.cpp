// Testing contentmanager_fixed
#include "catch.hpp"
#include "contentmanager_fixed.h"

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
    FixedMockProtocol() {
    }
    virtual bool read(std::vector<char> & data) override {
        if (fixedDoExit) {
            return false;
        }
        fixedAssignOrder(fixedReadOrder);
        REQUIRE(data.size() == 8);
        memcpy(&data[0], "TRAF", 4);
        uint32_t size = htonl(8);
        memcpy(&data[4], &size, sizeof(uint32_t));
        fixedDoExit = true;
        return true;
    };
    virtual void close() override {
        fixedDoExit = true;
    }
    
    virtual bool write(const std::vector<char> & data) override {
        fixedLastWrite = data;
        fixedAssignOrder(fixedWriteOrder);
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
    }
}
