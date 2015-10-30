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
        if (doExit) {
            return false;
        }
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
        return true;
    }
    std::atomic<bool> doExit;
};

TEST_CASE("Test random generating random text", "[content]") {
    SECTION("Test text generated") {
        std::unique_ptr<Protocol> proto(new MockProtocol());
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
    }
}

// TODO test client and server mode