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
#include "catch.hpp"
#include "hippomocks.h"
#include "lib/server.h"
#include "lib/common.h"

TEST_CASE("Server", "[protocol][server]") {
    SECTION("Working") {
        class MockProtocol : public Protocol {
        public:
            MockProtocol() : Protocol("Mock"), returnedOne(false) {
            }
            virtual bool listen(const Host & ignoredHost, const int backlog) override { UNUSED(ignoredHost); UNUSED(backlog); return true; };
            virtual std::unique_ptr<Protocol> waitForNewConnection() override {
                if (!returnedOne) {
                    returnedOne = true;
                    return std::unique_ptr<Protocol>(new MockProtocol());
                } else {
                    return std::unique_ptr<Protocol>(new Protocol("Mock"));
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
            virtual bool Stop() override {
                contentManagerCounterStop++;
                return true;
            };
            virtual bool Start() override {
                contentManagerCounterStart++;
                return true;
            }
            virtual void setMinimumSize(unsigned long size) noexcept override {
                UNUSED(size);
            };
            virtual void setMaximumSize(unsigned long size) noexcept override {
                UNUSED(size);
            };
            virtual ContentManagerType getType() const noexcept override {
                return ContentManagerType::None;
            }
        };
        static std::unique_ptr<CommonHeaders> commonHeaders(new CommonHeaders());
        class MockContentManagerFactory: public ContentManagerFactory {
        public:
            MockContentManagerFactory(std::shared_ptr<ContentManagerCustomizer> & contentManagerCustomizer) : ContentManagerFactory(ContentManagerType::None, commonHeaders, contentManagerCustomizer) {;};
            virtual std::unique_ptr<ContentManager> createContentManager(std::unique_ptr<Protocol> protocol, bool isServer) override {
                UNUSED(protocol);
                UNUSED(isServer);
                return std::unique_ptr<ContentManager>(new MockContentManager());
            };
        };
        MockProtocolFactory mockProtocolFactory;
        std::shared_ptr<ContentManagerCustomizer> contentManagerCustomizer(new ContentManagerCustomizer(100, 100000));
        std::shared_ptr<ContentManagerFactory>  mockContentManagerFactory(new  MockContentManagerFactory(contentManagerCustomizer));
        MockRepository mocks;
        {
            Server testServer;
            REQUIRE(testServer.getPorts().empty());
            Host testHost("0.0.0.0", 80, Host::ProtocolPreference::IPV4);
            REQUIRE(testServer.addPort(1, testHost, mockProtocolFactory, mockContentManagerFactory));
            REQUIRE_FALSE(testServer.addPort(1, testHost, mockProtocolFactory, mockContentManagerFactory));
            REQUIRE(testServer.getPorts().size() == 1);
            // test stopPorts
            REQUIRE(testServer.stopPort(1));
            REQUIRE(testServer.getPorts().empty());
            // test adding a port after stopping it
            REQUIRE(testServer.addPort(1, testHost, mockProtocolFactory, mockContentManagerFactory));
            REQUIRE(testServer.getPorts().size() == 1);
            // test adding a second port
            Host testHost2("0.0.0.0", 81, Host::ProtocolPreference::IPV4);
            REQUIRE(testServer.addPort(2, testHost2, mockProtocolFactory, mockContentManagerFactory));
            REQUIRE(testServer.getPorts().size() == 2);
            REQUIRE(testServer.stopPort(1));
            REQUIRE(testServer.getPorts().size() == 1);
            REQUIRE(testServer.getPorts()[0] == 2);
            
            REQUIRE(testServer.getNumBytesRead() == 0);
            REQUIRE(testServer.getNumBytesWritten() == 0);
            REQUIRE(testServer.getNumServers() == 1);
            
            nlohmann::json json = testServer.toJson();
            REQUIRE(json.size() == 2);
            
            REQUIRE(json["numListeners"].get<unsigned>() == 1);
            REQUIRE(json["listeners"].is_array());
            REQUIRE(json["listeners"].size() == 1);
            
            nlohmann::json json2 = testServer.toJson(2);
            REQUIRE(json2.size() == 2);
            REQUIRE(json2["found"].get<bool>() == true);
            REQUIRE(json2["listener"].is_object());
            
            nlohmann::json json3 = testServer.toJson(1);
            REQUIRE(json3.size() == 1);
            REQUIRE(json3["found"].get<bool>() == false);
        }
        REQUIRE(contentManagerCounterStart == 3);
        REQUIRE(contentManagerCounterStop == 3);
    }
}
