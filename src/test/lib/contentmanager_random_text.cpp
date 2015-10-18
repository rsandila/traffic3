// Testing contentmanager_random_text
#include <thread>
#include "catch.hpp"
#include "protocol.h"
#include "contentmanager_random_text.h"

class MockProtocol: public Protocol {
public:
    MockProtocol() : blockReadUntilReady(false), doExit(false) {
    }
    virtual bool read(std::vector<char> & data) override {
        // TODO: adjust when we start comparing headers
        while (!blockReadUntilReady) {
            if (doExit) {
                return false;
            }
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
        blockReadUntilReady = false;
        data.resize(4);
        memcpy(&data[0], "TR00", 4);
           return true;
    };
    virtual void close() override {
        doExit = true;
    }

    virtual bool write(std::vector<char> & data) override {
        lastWrite = data;
        return true;
    }
    std::atomic<bool> blockReadUntilReady;
    std::atomic<bool> doExit;
    std::vector<char> lastWrite;
};

TEST_CASE("Test random generating random text", "[content]") {
    SECTION("Test text generated") {
        MockProtocol proto;
        
        ContentManager_Random_Text manager(proto);
        manager.setMinimumSize(10);
        manager.setMaximumSize(20);
        proto.blockReadUntilReady = true;
        while (proto.blockReadUntilReady) {
            std::this_thread::sleep_for(std::chrono::microseconds(500));
        }
        proto.doExit = true;
        REQUIRE(proto.lastWrite.size() >= 10);
        REQUIRE(proto.lastWrite.size() <= 20);
    }
}