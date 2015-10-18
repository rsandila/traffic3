// Testing contentmanager_random_text
#include <thread>
#include "catch.hpp"
#include "protocol.h"
#include "contentmanager_random_text.h"

std::vector<char> lastWrite;

class MockProtocol: public Protocol {
public:
    MockProtocol() : doExit(false) {
    }
    virtual bool read(std::vector<char> & data) override {
        // TODO: adjust when we start comparing headers
        if (doExit) {
            return false;
        }
        data.resize(4);
        memcpy(&data[0], "TR00", 4);
        doExit = true;
        return true;
    };
    virtual void close() override {
        doExit = true;
    }

    virtual bool write(const std::vector<char> & data) override {
        lastWrite = data;
        return true;
    }
    std::atomic<bool> doExit;
};

TEST_CASE("Test random generating random text", "[content]") {
    SECTION("Test text generated") {
        std::unique_ptr<Protocol> proto(new MockProtocol());
        ContentManager_Random_Text manager(std::move(proto));
        manager.setMinimumSize(10);
        manager.setMaximumSize(20);
        REQUIRE(manager.Start());
        while (lastWrite.size() == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        REQUIRE(lastWrite.size() >= 10);
        REQUIRE(lastWrite.size() <= 20);
    }
}