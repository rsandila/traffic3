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
    SECTION("Bad protocolFactory") {
        ProtocolFactory protocolFactory(ProtocolType::None);
        std::unique_ptr<CommonHeaders> commonHeaders(new CommonHeaders());
        
        std::shared_ptr<ContentManagerCustomizer> contentManagerCustomizer(new ContentManagerCustomizer(100, 100000));
        std::shared_ptr<ContentManagerFactory> contentManagerFactory(new ContentManagerFactory(ContentManagerType::None, commonHeaders, contentManagerCustomizer));
        Listener listener(1, Host::ALL_INTERFACES4, protocolFactory, contentManagerFactory);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        REQUIRE(listener.inErrorState());
    }
    SECTION("Compare to Host") {
        Host googleDNS("google-public-dns-a.google.com", 80, Host::ProtocolPreference::IPV4);
        ProtocolFactory protocolFactory(ProtocolType::None);

        std::unique_ptr<CommonHeaders> commonHeaders(new CommonHeaders());
        std::shared_ptr<ContentManagerCustomizer> contentManagerCustomizer(new ContentManagerCustomizer(100, 100000));
        std::shared_ptr<ContentManagerFactory> contentManagerFactory(new ContentManagerFactory(ContentManagerType::None, commonHeaders, contentManagerCustomizer));
        Listener listener(1, Host::ALL_INTERFACES4, protocolFactory, contentManagerFactory);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        REQUIRE(listener == 1);
        REQUIRE(listener.getHost() == Host::ALL_INTERFACES4);
        REQUIRE_FALSE(listener == 2);
    }
}


TEST_CASE("Listener test", "[server]") {
    SECTION("Listen success, waitForNewConnection fails") {
        class MockProtocol : public Protocol {
        public:
            virtual bool listen(const Host & ignoredHost, const int backlog) override { UNUSED(ignoredHost); UNUSED(backlog); return true; };
        };
        class MockProtocolFactory: public ProtocolFactory {
        public:
            MockProtocolFactory() : ProtocolFactory(ProtocolType::None) {;}
            virtual std::unique_ptr<Protocol> createProtocol() {
                return std::unique_ptr<Protocol>(new MockProtocol);
            }
        };
        static std::unique_ptr<CommonHeaders> commonHeaders(new CommonHeaders());
        class MockContentManagerFactory: public ContentManagerFactory {
        public:
            MockContentManagerFactory(std::shared_ptr<ContentManagerCustomizer> & contentManagerCustomizer) : ContentManagerFactory(ContentManagerType::None, commonHeaders, contentManagerCustomizer) {
            };
            virtual std::unique_ptr<ContentManager> createContentManager(std::unique_ptr<Protocol> protocol, bool isServer) override {
                UNUSED(protocol);
                UNUSED(isServer);
                FAIL("Should never call createContentManager");
                return std::unique_ptr<ContentManager>(nullptr);
            }
        };
        MockProtocolFactory mockProtocolFactory;
        std::shared_ptr<ContentManagerCustomizer> contentManagerCustomizer(new ContentManagerCustomizer(100, 100000));
        std::shared_ptr<ContentManagerFactory>  contentManagerFactory(new  MockContentManagerFactory(contentManagerCustomizer));
        Listener listener(1, Host::ALL_INTERFACES4, mockProtocolFactory, contentManagerFactory);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        REQUIRE_FALSE(listener.inErrorState());
    }
    SECTION("Listen success, waitForNewConnection success, createContentManager fails") {
        class MockProtocol : public Protocol {
        public:
            MockProtocol() : shouldReturnConnection(true) {;};
            virtual bool listen(const Host & ignoredHost, const int backlog) override { UNUSED(ignoredHost); UNUSED(backlog); return true; };
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
        static std::unique_ptr<CommonHeaders> commonHeaders(new CommonHeaders());
        class MockContentManagerFactory: public ContentManagerFactory {
        public:
            MockContentManagerFactory(std::shared_ptr<ContentManagerCustomizer> & contentManagerCustomizer) : ContentManagerFactory(ContentManagerType::None, commonHeaders, contentManagerCustomizer) {
            };
            virtual std::unique_ptr<ContentManager> createContentManager(std::unique_ptr<Protocol> protocol, bool isServer) override {
                UNUSED(protocol); UNUSED(isServer);
                return std::unique_ptr<ContentManager>(nullptr);
            }
        };
        MockProtocolFactory mockProtocolFactory;
        std::shared_ptr<ContentManagerCustomizer> contentManagerCustomizer(new ContentManagerCustomizer(100, 100000));
        std::shared_ptr<ContentManagerFactory>  contentManagerFactory(new  MockContentManagerFactory(contentManagerCustomizer));
        Listener listener(1, Host::ALL_INTERFACES4, mockProtocolFactory, contentManagerFactory);
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
            virtual bool listen(const Host & ignoredHost, const int backlog) override { UNUSED(ignoredHost); UNUSED(backlog); return true; };
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
        static std::unique_ptr<CommonHeaders> commonHeaders(new CommonHeaders());
        class MockContentManagerFactory: public ContentManagerFactory {
        public:
            MockContentManagerFactory(std::shared_ptr<ContentManagerCustomizer> & contentManagerCustomizer) : ContentManagerFactory(ContentManagerType::None, commonHeaders, contentManagerCustomizer) {
            };
            virtual std::unique_ptr<ContentManager> createContentManager(std::unique_ptr<Protocol> protocol, bool isServer) override {
                UNUSED(protocol); UNUSED(isServer);
                return std::unique_ptr<ContentManager>(new MockContentManager());
            }
        };
        {
            MockProtocolFactory mockProtocolFactory;
            std::shared_ptr<ContentManagerCustomizer> contentManagerCustomizer(new ContentManagerCustomizer(100, 100000));
            std::shared_ptr<ContentManagerFactory>  contentManagerFactory(new  MockContentManagerFactory(contentManagerCustomizer));
            Listener listener(1, Host::ALL_INTERFACES4, mockProtocolFactory, contentManagerFactory);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            REQUIRE_FALSE(listener.inErrorState());
        }
        REQUIRE(startCalled == 1);
        REQUIRE(stopCalled == 1);
    }
}

// TODO - test toJSon
// TODO - test unsigned getPortId() const noexcept;
// TODO - test long long getBytesRead() const noexcept;
// TODO - test long long getBytesWritten() const noexcept;
