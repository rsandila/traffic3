// Tests for Listener class
#include "catch.hpp"
#include "listener.h"
#include "protocolfactory.h"

TEST_CASE("Listener: Unable to listen", "[server]") {
    SECTION("Bad protocolFactory") {
        ProtocolFactory protocolFactory(ProtocolType::None);
        ContentManagerFactory contentManagerFactory(ContentManagerType::None);
        Listener listener(Host::ALL_INTERFACES, protocolFactory, contentManagerFactory);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        REQUIRE(listener.inErrorState());
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    SECTION("Compare to Host") {
        Host googleDNS("google-public-dns-a.google.com", 80);
        ProtocolFactory protocolFactory(ProtocolType::None);
        ContentManagerFactory contentManagerFactory(ContentManagerType::None);
        Listener listener(Host::ALL_INTERFACES, protocolFactory, contentManagerFactory);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        REQUIRE(listener == Host::ALL_INTERFACES);
        REQUIRE(listener.getHost() == Host::ALL_INTERFACES);
        REQUIRE_FALSE(listener == googleDNS);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}


TEST_CASE("Listener test", "[server]") {
    SECTION("Listen success, waitForNewConnection fails") {
        class MockProtocol : public Protocol {
        public:
            virtual bool listen(const Host & host) { return true; };
        };
        class MockProtocolFactory: public ProtocolFactory {
        public:
            MockProtocolFactory() : ProtocolFactory(ProtocolType::None) {;}
            virtual std::unique_ptr<Protocol> createProtocol() {
                return std::unique_ptr<Protocol>(new MockProtocol);
            }
        };
        class MockContentManagerFactory: public ContentManagerFactory {
        public:
            MockContentManagerFactory() : ContentManagerFactory(ContentManagerType::None) {
            };
            virtual std::unique_ptr<ContentManager> createContentManager(std::unique_ptr<Protocol> protocol) {
                FAIL("Should never call createContentManager");
                return std::unique_ptr<ContentManager>(nullptr);
            }
        };
        MockProtocolFactory mockProtocolFactory;
        MockContentManagerFactory contentManagerFactory;
        Listener listener(Host::ALL_INTERFACES, mockProtocolFactory, contentManagerFactory);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        REQUIRE_FALSE(listener.inErrorState());
    }
    SECTION("Listen success, waitForNewConnection success, createContentManager fails") {
        class MockProtocol : public Protocol {
        public:
            virtual bool listen(const Host & host) { return true; };
            virtual std::unique_ptr<Protocol> waitForNewConnection() { return std::unique_ptr<Protocol>(new MockProtocol()); };
            virtual ProtocolState getState() {
                return Protocol::ProtocolState::OPEN;
            }
        };
        class MockProtocolFactory: public ProtocolFactory {
        public:
            MockProtocolFactory() : ProtocolFactory(ProtocolType::None) {;}
            virtual std::unique_ptr<Protocol> createProtocol() {
                return std::unique_ptr<Protocol>(new MockProtocol);
            }
        };
        class MockContentManagerFactory: public ContentManagerFactory {
        public:
            MockContentManagerFactory() : ContentManagerFactory(ContentManagerType::None) {
            };
            virtual std::unique_ptr<ContentManager> createContentManager(std::unique_ptr<Protocol> protocol) {
                return std::unique_ptr<ContentManager>(nullptr);
            }
        };
        MockProtocolFactory mockProtocolFactory;
        MockContentManagerFactory contentManagerFactory;
        Listener listener(Host::ALL_INTERFACES, mockProtocolFactory, contentManagerFactory);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        REQUIRE_FALSE(listener.inErrorState());        
    }
    SECTION("Listen success, waitForNewConnection success, createContentManager success, start called") {
        static int startCalled = 0;
        static int stopCalled = 0;

        class MockProtocol : public Protocol {
        public:
            MockProtocol() : returnedOne(false) {
            }
            virtual bool listen(const Host & host) { return true; };
            virtual std::unique_ptr<Protocol> waitForNewConnection() {
                if (!returnedOne) {
                    returnedOne = true;
                    return std::unique_ptr<Protocol>(new MockProtocol());
                } else {
                    return std::unique_ptr<Protocol>(new Protocol());
                }
            };
            virtual ProtocolState getState() {
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
        class MockContentManager : public ContentManager {
        public:
            virtual bool Stop() {
                stopCalled++;
                return true;
            };
            virtual bool Start() {
                startCalled++;
                return true;
            }
        };
        class MockContentManagerFactory: public ContentManagerFactory {
        public:
            MockContentManagerFactory() : ContentManagerFactory(ContentManagerType::None) {
            };
            virtual std::unique_ptr<ContentManager> createContentManager(std::unique_ptr<Protocol> protocol) {
                return std::unique_ptr<ContentManager>(new MockContentManager());
            }
        };
        {
            MockProtocolFactory mockProtocolFactory;
            MockContentManagerFactory contentManagerFactory;
            Listener listener(Host::ALL_INTERFACES, mockProtocolFactory, contentManagerFactory);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            REQUIRE_FALSE(listener.inErrorState());
        }
        REQUIRE(startCalled == 1);
        REQUIRE(stopCalled == 1);
    }
}

