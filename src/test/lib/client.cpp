/*
 * Copyright (C) 2016 Robert Sandilands
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
#include "lib/client.h"

TEST_CASE("Client", "[protocol][client]") {
    SECTION("Working") {
        class MockProtocol : public Protocol {
        public:
            MockProtocol() {
            }
            virtual bool connect(const Host & localHost) override { UNUSED(localHost); return true; };
            virtual ProtocolState getState() override {
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
            virtual nlohmann::json toJson() const noexcept override {
                nlohmann::json json;
                json["name"] = "MockContentManager";
                return json;
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
            virtual nlohmann::json toJson() const noexcept override {
                nlohmann::json json;
                json["name"] = "MockContentManagerFactory";
                return json;
            }
        };
        std::shared_ptr<ContentManagerCustomizer> contentManagerCustomizer(new ContentManagerCustomizer(100, 100000));
        MockContentManagerFactory  mockContentManagerFactory(contentManagerCustomizer);

        Client client;
        MockProtocolFactory mockProtocolFactory;
        REQUIRE(client.getNumClients() == 0);
        REQUIRE(client.getNumBytesRead() == 0);
        REQUIRE(client.getNumBytesWritten() == 0);
        Host server("::", 8080, Host::ProtocolPreference::ANY);
        REQUIRE(client.startClients(1, 1, mockProtocolFactory, mockContentManagerFactory, server));
        REQUIRE(client.getNumClients() == 1);
        REQUIRE_FALSE(client.startClients(1, 1, mockProtocolFactory, mockContentManagerFactory, server));
        REQUIRE(client.getNumClients() == 1);
        REQUIRE(client.stopClients(1));
        REQUIRE(client.getNumClients() == 0);
        REQUIRE_FALSE(client.stopClients(1));
        REQUIRE(client.getNumClients() == 0);
        REQUIRE(client.startClients(1, 1, mockProtocolFactory, mockContentManagerFactory, server));
        REQUIRE(client.getNumClients() == 1);
        REQUIRE(client.startClients(2, 2, mockProtocolFactory, mockContentManagerFactory, server));
        REQUIRE(client.getNumClients() == 2);
        REQUIRE(client.stopClients(1));
        REQUIRE(client.getNumClients() == 1);
        REQUIRE(client.getNumBytesRead() == 0);
        REQUIRE(client.getNumBytesWritten() == 0);
        
        nlohmann::json json = client.toJson();
        REQUIRE(json.size() == 2);
        REQUIRE(json["numClients"].get<unsigned>() == 1);
        REQUIRE(json["workers"].is_object());
        REQUIRE(json["workers"].size() == 1);
        REQUIRE(json["workers"]["2"].is_array());
        REQUIRE(json["workers"]["2"].size() == 2);
        REQUIRE(json["workers"]["2"][0].is_object());
        REQUIRE(json["workers"]["2"][1].is_object());
        
        nlohmann::json json2 = client.toJson(2);
        REQUIRE(json2["found"].get<bool>());
        REQUIRE(json2["worker"].is_array());
        REQUIRE(json2["worker"].size() == 2);
        REQUIRE(json2["worker"][0].is_object());
        REQUIRE(json2["worker"][1].is_object());
        
        REQUIRE(client.stopClients(2));
        REQUIRE(client.getNumClients() == 0);
    }
}