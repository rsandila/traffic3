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
// Tests for Listener class
#include "catch.hpp"
#include "lib/listener.h"
#include "protocol/protocolfactory.h"

TEST_CASE("Listener: Unable to listen", "[server]") {
    CommonHeaders commonHeaders;
    SECTION("Bad protocolFactory") {
        ProtocolFactory protocolFactory(ProtocolType::None);
        ContentManagerFactory contentManagerFactory(ContentManagerType::None, 100, 10000, commonHeaders);
        Listener listener(Host::ALL_INTERFACES4, protocolFactory, contentManagerFactory);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        REQUIRE(listener.inErrorState());
    }
    SECTION("Compare to Host") {
        Host googleDNS("google-public-dns-a.google.com", 80, Host::ProtocolPreference::IPV4);
        ProtocolFactory protocolFactory(ProtocolType::None);
        ContentManagerFactory contentManagerFactory(ContentManagerType::None, 100, 10000, commonHeaders);
        Listener listener(Host::ALL_INTERFACES4, protocolFactory, contentManagerFactory);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        REQUIRE(listener == Host::ALL_INTERFACES4);
        REQUIRE(listener.getHost() == Host::ALL_INTERFACES4);
        REQUIRE_FALSE(listener == googleDNS);
    }
}


TEST_CASE("Listener test", "[server]") {
    SECTION("Listen success, waitForNewConnection fails") {
        class MockProtocol : public Protocol {
        public:
            virtual bool listen(const Host & host, const int backlog) override { UNUSED(host); UNUSED(backlog); return true; };
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
            CommonHeaders header;
            MockContentManagerFactory() : ContentManagerFactory(ContentManagerType::None, 100, 10000, header) {
            };
            virtual std::unique_ptr<ContentManager> createContentManager(std::unique_ptr<Protocol> protocol, bool isServer) override {
                UNUSED(protocol);
                UNUSED(isServer);
                FAIL("Should never call createContentManager");
                return std::unique_ptr<ContentManager>(nullptr);
            }
        };
        MockProtocolFactory mockProtocolFactory;
        MockContentManagerFactory contentManagerFactory;
        Listener listener(Host::ALL_INTERFACES4, mockProtocolFactory, contentManagerFactory);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        REQUIRE_FALSE(listener.inErrorState());
    }
    SECTION("Listen success, waitForNewConnection success, createContentManager fails") {
        class MockProtocol : public Protocol {
        public:
            MockProtocol() : shouldReturnConnection(true) {;};
            virtual bool listen(const Host & host, const int backlog) override { UNUSED(host); UNUSED(backlog); return true; };
            virtual std::unique_ptr<Protocol> waitForNewConnection() override {
                if (shouldReturnConnection) {
                    shouldReturnConnection = false;
                    return std::unique_ptr<Protocol>(new MockProtocol());
                } else {
                    return std::unique_ptr<Protocol>(nullptr);
                }
            }
            virtual ProtocolState getState() override {
                return Protocol::ProtocolState::OPEN;
            }
            virtual void close() override {
                shouldReturnConnection = false;
            }
            bool shouldReturnConnection;
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
            MockContentManagerFactory() : ContentManagerFactory(ContentManagerType::None, 100, 10000, commonHeaders) {
            };
            virtual std::unique_ptr<ContentManager> createContentManager(std::unique_ptr<Protocol> protocol, bool isServer) override {
                UNUSED(protocol); UNUSED(isServer);
                return std::unique_ptr<ContentManager>(nullptr);
            }
            CommonHeaders commonHeaders;
        };
        MockProtocolFactory mockProtocolFactory;
        MockContentManagerFactory contentManagerFactory;
        Listener listener(Host::ALL_INTERFACES4, mockProtocolFactory, contentManagerFactory);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        REQUIRE_FALSE(listener.inErrorState());        
    }
    SECTION("Listen success, waitForNewConnection success, createContentManager success, start called") {
        static int startCalled = 0;
        static int stopCalled = 0;

        class MockProtocol : public Protocol {
        public:
            MockProtocol() : returnedOne(false) {
            }
            virtual bool listen(const Host & host, const int backlog) override { UNUSED(host); UNUSED(backlog); return true; };
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
            MockContentManagerFactory() : ContentManagerFactory(ContentManagerType::None, 100, 10000, commonHeaders) {
            };
            virtual std::unique_ptr<ContentManager> createContentManager(std::unique_ptr<Protocol> protocol, bool isServer) override {
                UNUSED(protocol); UNUSED(isServer);
                return std::unique_ptr<ContentManager>(new MockContentManager());
            }
            CommonHeaders commonHeaders;
        };
        {
            MockProtocolFactory mockProtocolFactory;
            MockContentManagerFactory contentManagerFactory;
            Listener listener(Host::ALL_INTERFACES4, mockProtocolFactory, contentManagerFactory);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            REQUIRE_FALSE(listener.inErrorState());
        }
        REQUIRE(startCalled == 1);
        REQUIRE(stopCalled == 1);
    }
}

