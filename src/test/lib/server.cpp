#include "catch.hpp"
#include "hippomocks.h"
#include "server.h"

TEST_CASE("Server", "[protocol][server]") {
    SECTION("Working") {
        class MockProtocol : public Protocol {
        public:
            MockProtocol() : returnedOne(false) {
            }
            virtual bool listen(const Host & host, const int backlog) override { return true; };
            virtual std::unique_ptr<Protocol> waitForNewConnection() override {
                if (!returnedOne) {
                    returnedOne = true;
                    return std::unique_ptr<Protocol>(new MockProtocol());
                } else {
                    return std::unique_ptr<Protocol>(new Protocol());
                }
            };
            virtual ProtocolState getState() override {
                return Protocol::ProtocolState::OPEN;
            }
            bool returnedOne;
        };
        class MockProtocolFactory: public ProtocolFactory {
        public:
            MockProtocolFactory() : ProtocolFactory(ProtocolType::None) {;}
            virtual std::unique_ptr<Protocol> createProtocol() {
                return std::unique_ptr<Protocol>(new MockProtocol);
            }
        };
        static int contentManagerCounterStart = 0;
        static int contentManagerCounterStop = 0;
        class MockContentManager: public ContentManager {
            virtual bool Stop() {
                contentManagerCounterStop++;
                return true;
            };
            virtual bool Start() {
                contentManagerCounterStart++;
                return true;
            }
            virtual void setMinimumSize(unsigned size) noexcept {
            };
            virtual void setMaximumSize(unsigned size) noexcept {
            };
            virtual ContentManagerType getType() const noexcept {
                return ContentManagerType::None;
            }
        };
        class MockContentManagerFactory: public ContentManagerFactory {
        public:
            MockContentManagerFactory() : ContentManagerFactory(ContentManagerType::None, 100, 10000, CommonHeaders()) {;};
            virtual std::unique_ptr<ContentManager> createContentManager(std::unique_ptr<Protocol> protocol, bool isServer) override {
                return std::unique_ptr<ContentManager>(new MockContentManager());
            };
            
        };
        MockProtocolFactory mockProtocolFactory;
        MockContentManagerFactory mockContentManagerFactory;
        MockRepository mocks;
        {
            Server testServer(mockProtocolFactory, mockContentManagerFactory);
            REQUIRE(testServer.getPorts().empty());
            Host testHost("0.0.0.0", 80);
            REQUIRE(testServer.addPort(testHost));
            REQUIRE_FALSE(testServer.addPort(testHost));
            REQUIRE(testServer.getPorts().size() == 1);
            // test stopPorts
            REQUIRE(testServer.stopPort(testHost));
            REQUIRE(testServer.getPorts().empty());
            // test adding a port after stopping it
            REQUIRE(testServer.addPort(testHost));
            REQUIRE(testServer.getPorts().size() == 1);
            // test adding a second port
            Host testHost2("0.0.0.0", 81);
            REQUIRE(testServer.addPort(testHost2));
            REQUIRE(testServer.getPorts().size() == 2);
            REQUIRE(testServer.stopPort(testHost));
            REQUIRE(testServer.getPorts().size() == 1);
            REQUIRE(testServer.getPorts()[0] == testHost2);
        }
        REQUIRE(contentManagerCounterStart == 3);
        REQUIRE(contentManagerCounterStop == 3);
    }
}