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
// Testing CommonHeaders
#include <memory.h>
#include "catch.hpp"
#include "lib/commonheaders.h"
#include "lib/common.h"

TEST_CASE("CommonHeaders", "[headers]") {
    SECTION("Read returns invalid signature") {
        class MockProtocol : public Protocol {
        public:
            virtual bool read(std::vector<char> & data, bool allowPartialRead, Host & hostState) {
                data.resize(8);
                data[0] = 'A';
                data[1] = 'B';
                data[2] = 'C';
                data[3] = 'D';
                uint32_t length = htonl(8);
                memcpy(&data[4], &length, sizeof(uint32_t));
                UNUSED(allowPartialRead); UNUSED(hostState);
                return true; };
        };
        CommonHeaders headers;
		std::unique_ptr<Protocol> mockProtocol(new MockProtocol());
        std::vector<char> data;
        data.resize(1024);
        Host host = Host::ALL_INTERFACES4;
        REQUIRE_FALSE(headers.read(mockProtocol, data, host));
    }
    SECTION("Read returns short") {
        class MockProtocol : public Protocol {
        public:
            virtual bool read(std::vector<char> & data, bool allowPartialRead, Host & hostState) {
                data.resize(5);
                data[0] = 'T';
                data[1] = 'R';
                data[2] = 'A';
                data[3] = 'F';
                data[4] = 0;
                UNUSED(allowPartialRead); UNUSED(hostState);
                return true; };
        };
        CommonHeaders headers;
		std::unique_ptr<Protocol> mockProtocol(new MockProtocol());
        std::vector<char> data;
        data.resize(1024);
        Host host = Host::ALL_INTERFACES4;
        REQUIRE_FALSE(headers.read(mockProtocol, data, host));
    }
    SECTION("Read returns invalid length") {
        class MockProtocol : public Protocol {
        public:
            virtual bool read(std::vector<char> & data, bool allowPartialRead, Host & hostState) {
                data.resize(8);
                data[0] = 'A';
                data[1] = 'B';
                data[2] = 'C';
                data[3] = 'D';
                uint32_t length = htonl(3);
                memcpy(&data[4], &length, sizeof(uint32_t));
                UNUSED(allowPartialRead); UNUSED(hostState);
                return true; };
        };
        CommonHeaders headers;
		std::unique_ptr<Protocol> mockProtocol(new MockProtocol());
		std::vector<char> data;
        data.resize(1024);
        Host host = Host::ALL_INTERFACES4;
        REQUIRE_FALSE(headers.read(mockProtocol, data, host));
    }
    SECTION("First read fails") {
        class MockProtocol : public Protocol {
        public:
            virtual bool read(std::vector<char> & data, bool allowPartialRead, Host & hostState) {
                UNUSED(data); UNUSED(allowPartialRead); UNUSED(hostState);
                return false; };
        };
        CommonHeaders headers;
		std::unique_ptr<Protocol> mockProtocol(new MockProtocol());
		std::vector<char> data;
        data.resize(1024);
        Host host = Host::ALL_INTERFACES4;
        REQUIRE_FALSE(headers.read(mockProtocol, data, host));
    }
    SECTION("Second read failes") {
        class MockProtocol : public Protocol {
        public:
            MockProtocol() : firstTime(true) {
            }
            virtual bool read(std::vector<char> & data, bool allowPartialRead, Host & hostState) {
                UNUSED(allowPartialRead); UNUSED(hostState);
                if (firstTime) {
                    data.resize(8);
                    data[0] = 'T';
                    data[1] = 'R';
                    data[2] = 'A';
                    data[3] = 'F';
                    uint32_t length = htonl(20);
                    memcpy(&data[4], &length, sizeof(uint32_t));
                    firstTime = false;
                    return true;
                } else {
                    return false;
                }
            }
            bool firstTime;
        };
        CommonHeaders headers;
		std::unique_ptr<Protocol> mockProtocol(new MockProtocol());
		std::vector<char> data;
        data.resize(1024);
        Host host = Host::ALL_INTERFACES4;
        REQUIRE_FALSE(headers.read(mockProtocol, data, host));
    }
    SECTION("Read header only") {
        class MockProtocol : public Protocol {
        public:
            virtual bool read(std::vector<char> & data, bool allowPartialRead, Host & hostState) {
                data.resize(8);
                data[0] = 'T';
                data[1] = 'R';
                data[2] = 'A';
                data[3] = 'F';
                uint32_t length = htonl(8);
                memcpy(&data[4], &length, sizeof(uint32_t));
                UNUSED(allowPartialRead); UNUSED(hostState);
                return true; };
        };
        CommonHeaders headers;
		std::unique_ptr<Protocol> mockProtocol(new MockProtocol());
		std::vector<char> data;
        data.resize(1024);
        Host host = Host::ALL_INTERFACES4;
        REQUIRE(headers.read(mockProtocol, data, host));
        REQUIRE(data.size() == 0);
    }
    SECTION("Read header plus data") {
        class MockProtocol : public Protocol {
        public:
            MockProtocol() : firstTime(true) {
            }
            virtual bool read(std::vector<char> & data, bool allowPartialRead, Host & hostState) {
                UNUSED(allowPartialRead); UNUSED(hostState);
                if (firstTime) {
                    data.resize(8);
                    data[0] = 'T';
                    data[1] = 'R';
                    data[2] = 'A';
                    data[3] = 'F';
                    uint32_t length = htonl(10);
                    memcpy(&data[4], &length, sizeof(uint32_t));
                    firstTime = false;
                    return true;
                } else {
                    data.resize(2);
                    data[0] = 'A';
                    data[1] = 'B';
                    return true;
                }
            }
            bool firstTime;
        };
        CommonHeaders headers;
		std::unique_ptr<Protocol> mockProtocol(new MockProtocol());
		std::vector<char> data;
        data.resize(1024);
        Host host = Host::ALL_INTERFACES4;
        REQUIRE(headers.read(mockProtocol, data, host));
        REQUIRE(data.size() == 2);
        REQUIRE(data[0] == 'A');
        REQUIRE(data[1] == 'B');
    }
    SECTION("Write success") {
        class MockProtocol : public Protocol {
        public:
            MockProtocol() : count(0) {
            }
            virtual bool write(const std::vector<char> & data, const Host & hostState) {
                const uint32_t length = htonl(12);
                uint32_t actualLength;
                switch (++count) {
                    case 1:
                        REQUIRE(data.size() == 8);
                        REQUIRE(data[0] == 'T');
                        REQUIRE(data[1] == 'R');
                        REQUIRE(data[2] == 'A');
                        REQUIRE(data[3] == 'F');
                        actualLength = *(uint32_t *)(&data[4]);
                        REQUIRE(length == actualLength);
                        return true;
                    case 2:
                        REQUIRE(data.size() == 4);
                        REQUIRE(data[0] == 'A');
                        REQUIRE(data[1] == 'B');
                        REQUIRE(data[2] == 'C');
                        REQUIRE(data[3] == 'D');
                        return true;
                        
                }
                FAIL("Should never get here");
                UNUSED(hostState);
                return false;
            };
            int count;
        };
		std::unique_ptr<Protocol> mockProtocol(new MockProtocol());
		CommonHeaders headers;
        Host host = Host::ALL_INTERFACES4;
        std::vector<char> data;
        data.resize(4);
        data[0] = 'A';
        data[1] = 'B';
        data[2] = 'C';
        data[3] = 'D';
        REQUIRE(headers.write(mockProtocol, data, host));
        
        nlohmann::json json = headers.toJson();
        REQUIRE(json.size() == 2);
        
        REQUIRE(json["version"].get<unsigned>() == 2);
        REQUIRE(json["type"].get<std::string>() == "commonHeaders");
    }
    SECTION("Write success once") {
        class MockProtocol : public Protocol {
        public:
            MockProtocol() : count(0) {
            }
            virtual bool write(const std::vector<char> & data, const Host & hostState) {
                const uint32_t length = htonl(12);
                uint32_t actualLength;
                switch (++count) {
                    case 1:
                        REQUIRE(data.size() == 8);
                        REQUIRE(data[0] == 'T');
                        REQUIRE(data[1] == 'R');
                        REQUIRE(data[2] == 'A');
                        REQUIRE(data[3] == 'F');
                        actualLength = *(uint32_t *)(&data[4]);
                        REQUIRE(length == actualLength);
                        return true;
                    case 2:
                        REQUIRE(data.size() == 4);
                        REQUIRE(data[0] == 'A');
                        REQUIRE(data[1] == 'B');
                        REQUIRE(data[2] == 'C');
                        REQUIRE(data[3] == 'D');
                        return false;
                        
                }
                UNUSED(hostState);
                FAIL("Should never get here");
                return false;
            };
            int count;
        };
		std::unique_ptr<Protocol> mockProtocol(new MockProtocol());
		CommonHeaders headers;
        Host host = Host::ALL_INTERFACES4;
        std::vector<char> data;
        data.resize(4);
        data[0] = 'A';
        data[1] = 'B';
        data[2] = 'C';
        data[3] = 'D';
        REQUIRE_FALSE(headers.write(mockProtocol, data, host));
    }
    SECTION("Write never succeeds") {
        class MockProtocol : public Protocol {
        public:
            MockProtocol() : count(0) {
            }
            virtual bool write(const std::vector<char> & data, const Host & hostState) {
                const uint32_t length = htonl(12);
                uint32_t actualLength;
                switch (++count) {
                    case 1:
                        REQUIRE(data.size() == 8);
                        REQUIRE(data[0] == 'T');
                        REQUIRE(data[1] == 'R');
                        REQUIRE(data[2] == 'A');
                        REQUIRE(data[3] == 'F');
                        actualLength = *(uint32_t *)(&data[4]);
                        REQUIRE(length == actualLength);
                        return false;
                    case 2:
                        FAIL("Should never be called");
                        return false;                        
                }
                UNUSED(hostState);
                return false;
            };
            int count;
        };
		std::unique_ptr<Protocol> mockProtocol(new MockProtocol());
		CommonHeaders headers;
        Host host = Host::ALL_INTERFACES4;
        std::vector<char> data;
        data.resize(4);
        data[0] = 'A';
        data[1] = 'B';
        data[2] = 'C';
        data[3] = 'D';
        REQUIRE_FALSE(headers.write(mockProtocol, data, host));
    }
}

