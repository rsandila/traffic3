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
// Testing RestHeaders
#include <memory.h>
#include "catch.hpp"
#include "rest/restheaders.h"
#include "lib/common.h"

TEST_CASE("RestHeaders: write succeeds", "[headers]") {
    class MockProtocol : public Protocol {
    public:
        MockProtocol() : count(0) {
        }
        virtual bool write(const std::vector<char> & data, const Host & hostState) {
            switch (++count) {
                case 1:
                    REQUIRE(data.size() == 4);
                    REQUIRE(data[0] == 'A');
                    REQUIRE(data[1] == 'B');
                    REQUIRE(data[2] == 'C');
                    REQUIRE(data[3] == 'D');
                    return true;
                case 2:
                    FAIL("Should never be called");
                    return false;
            }
            UNUSED(hostState);
            FAIL("Should never get here");
            return false;
        }
        int count;
    };
	std::unique_ptr<Protocol> protocol(new MockProtocol());
    RestHeaders headers;
    Host host = Host::ALL_INTERFACES4;
    std::vector<char> data;
    data.resize(4);
    data[0] = 'A';
    data[1] = 'B';
    data[2] = 'C';
    data[3] = 'D';
    REQUIRE(headers.write(protocol, data, host));
}

TEST_CASE("RestHeaders: write fails", "[headers]") {
    class MockProtocol : public Protocol {
    public:
        MockProtocol() : count(0) {
        }
        virtual bool write(const std::vector<char> & data, const Host & hostState) {
            switch (++count) {
                case 1:
                    REQUIRE(data.size() == 4);
                    REQUIRE(data[0] == 'A');
                    REQUIRE(data[1] == 'B');
                    REQUIRE(data[2] == 'C');
                    REQUIRE(data[3] == 'D');
                    return false;
                case 2:
                    FAIL("Should never be called");
                    return false;
            }
            UNUSED(hostState);
            FAIL("Should never get here");
            return false;
        }
        int count;
    };
	std::unique_ptr<Protocol> protocol(new MockProtocol());
	RestHeaders headers;
    Host host = Host::ALL_INTERFACES4;
    std::vector<char> data;
    data.resize(4);
    data[0] = 'A';
    data[1] = 'B';
    data[2] = 'C';
    data[3] = 'D';
    REQUIRE_FALSE(headers.write(protocol, data, host));
}

TEST_CASE("RestHeaders: read returns 0", "[headers]") {
    class MockProtocol : public Protocol {
    public:
        virtual bool read(std::vector<char> & data, bool allowPartialRead, Host & hostState) {
            data.resize(0);
            UNUSED(allowPartialRead); UNUSED(hostState);
            return true;
        };
    };
	std::unique_ptr<Protocol> mockProtocol(new MockProtocol());
	RestHeaders headers;
    Host host = Host::ALL_INTERFACES4;
    std::vector<char> data;
    REQUIRE_FALSE(headers.read(mockProtocol, data, host));
}
TEST_CASE("RestHeaders: read returns no EOH", "[headers]") {
    class MockProtocol : public Protocol {
    public:
        MockProtocol() : counter(0) {
        }
        virtual bool read(std::vector<char> & data, bool allowPartialRead, Host & hostState) {
            const std::string testString = "This is a test\r\nSecond line\r\n";
            UNUSED(allowPartialRead); UNUSED(hostState);
            if (counter == 0) {
                data.resize(testString.length());
                memcpy(&data[0], testString.c_str(), testString.length());
            } else {
                data.resize(0);
            }
            counter += 1;
            return true;
        };
        int counter;
    };
	std::unique_ptr<Protocol> mockProtocol(new MockProtocol());
	RestHeaders headers;
    Host host = Host::ALL_INTERFACES4;
    std::vector<char> data;
    REQUIRE_FALSE(headers.read(mockProtocol, data, host));
}

TEST_CASE("RestHeaders: read returns EOH, but no Content-Length", "[headers]") {
    class MockProtocol : public Protocol {
    public:
        virtual bool read(std::vector<char> & data, bool allowPartialRead, Host & hostState) {
            const std::string testString = "This is a test\r\nSecond line\r\n\r\n";
            data.resize(testString.length());
            memcpy(&data[0], testString.c_str(), testString.length());
            UNUSED(allowPartialRead); UNUSED(hostState);
            return true;
        };
    };
	std::unique_ptr<Protocol> mockProtocol(new MockProtocol());
	RestHeaders headers;
    Host host = Host::ALL_INTERFACES4;
    std::vector<char> data;
    REQUIRE(headers.read(mockProtocol, data, host));
}

TEST_CASE("RestHeaders: read returns EOH, Content-Length == 0", "[headers]") {
    class MockProtocol : public Protocol {
    public:
        virtual bool read(std::vector<char> & data, bool allowPartialRead, Host & hostState) {
            const std::string testString = "This is a test\r\nSecond line\r\nContent-Length: 0\r\n\r\n";
            data.resize(testString.length());
            memcpy(&data[0], testString.c_str(), testString.length());
            UNUSED(allowPartialRead); UNUSED(hostState);
            return true;
        };
    };
	std::unique_ptr<Protocol> mockProtocol(new MockProtocol());
	RestHeaders headers;
    Host host = Host::ALL_INTERFACES4;
    std::vector<char> data;
    REQUIRE(headers.read(mockProtocol, data, host));
}

TEST_CASE("RestHeaders: read returns EOH, Content-Length == -1", "[headers]") {
    class MockProtocol : public Protocol {
    public:
        virtual bool read(std::vector<char> & data, bool allowPartialRead, Host & hostState) {
            const std::string testString = "This is a test\r\nSecond line\r\nContent-Length: -1\r\n\r\n";
            data.resize(testString.length());
            memcpy(&data[0], testString.c_str(), testString.length());
            UNUSED(allowPartialRead); UNUSED(hostState);
            return true;
        };
    };
	std::unique_ptr<Protocol> mockProtocol(new MockProtocol());
	RestHeaders headers;
    Host host = Host::ALL_INTERFACES4;
    std::vector<char> data;
    REQUIRE_FALSE(headers.read(mockProtocol, data, host));
}

TEST_CASE("RestHeaders: Read returns EOH, Content-Length == invalid", "[headers]") {
    class MockProtocol : public Protocol {
    public:
        virtual bool read(std::vector<char> & data, bool allowPartialRead, Host & hostState) {
            const std::string testString = "This is a test\r\nSecond line\r\nContent-Length: invalid\r\n\r\n";
            data.resize(testString.length());
            memcpy(&data[0], testString.c_str(), testString.length());
            UNUSED(allowPartialRead); UNUSED(hostState);
            return true;
        };
    };
	std::unique_ptr<Protocol> mockProtocol(new MockProtocol());
	RestHeaders headers;
    Host host = Host::ALL_INTERFACES4;
    std::vector<char> data;
    REQUIRE_FALSE(headers.read(mockProtocol, data, host));
}

TEST_CASE("RestHeaders: Read returns EOH, valid Content-Length, but no content", "[headers]") {
    class MockProtocol : public Protocol {
    public:
        MockProtocol() : counter(0) {
        }
        virtual bool read(std::vector<char> & data, bool allowPartialRead, Host & hostState) {
            // good return on first call, 0 return on second
            const std::string testString = "This is a test\r\nSecond line\r\nContent-Length: 100\r\n\r\n";
            counter += 1;
            switch (counter) {
                case 1:
                    data.resize(testString.length());
                    memcpy(&data[0], testString.c_str(), testString.length());
                    UNUSED(allowPartialRead); UNUSED(hostState);
                    return true;
                case 2:
                    data.resize(0);
                    return true;
                case 3:
                    return false;
            };
            FAIL("Should never get here");
            return false;
        };
        int counter = 0;
    };
	std::unique_ptr<Protocol> mockProtocol(new MockProtocol());
	RestHeaders headers;
    Host host = Host::ALL_INTERFACES4;
    std::vector<char> data;
    REQUIRE_FALSE(headers.read(mockProtocol, data, host));
}

TEST_CASE("RestHeaders: Read returns EOH, valid Content-Length and valid content in one read", "[headers]") {
    const std::string baseData = "This is a test\r\nSecond line\r\nContent-Length: ";
    const std::string testData = "0123456789testData";
    const std::string expectedData = baseData + std::to_string(testData.length()) +
    "\r\n\r\n" + testData;
        
    class MockProtocol : public Protocol {
    public:
        MockProtocol(const std::string & base, const std::string & testData) : counter(0),
        testString(base + std::to_string(testData.length()) +
                        "\r\n\r\n" + testData) {
        }
        virtual bool read(std::vector<char> & data, bool allowPartialRead, Host & hostState) {
            // good return on first call, 0 return on second
            counter += 1;
            switch (counter) {
                case 1:
                    data.resize(testString.length());
                    memcpy(&data[0], testString.c_str(), testString.length());
                    UNUSED(allowPartialRead); UNUSED(hostState);
                    return true;
                case 2:
                    FAIL("Should only be called once");
                    return false;
            };
            FAIL("Should never get here");
            return false;
        };
        int counter = 0;
        const std::string testString;
    };
	std::unique_ptr<Protocol> mockProtocol(new MockProtocol(baseData, testData));
    RestHeaders headers;
    Host host = Host::ALL_INTERFACES4;
    std::vector<char> data;
    REQUIRE(headers.read(mockProtocol, data, host));
    REQUIRE(std::string(&data[0], data.size()) == expectedData);
}

TEST_CASE("RestHeaders: Read returns EOH, valid Content-Length and valid content in two reads", "[headers]") {
    const std::string baseData = "This is a test\r\nSecond line\r\nContent-Length: ";
    const std::string testData = "0123456789testData";
    const std::string expectedData = baseData + std::to_string(testData.length()) +
        "\r\n\r\n" + testData;
    class MockProtocol : public Protocol {
    public:
        MockProtocol(const std::string & base, const std::string & testData) : counter(0),
        testString(base + std::to_string(testData.length()) +
                    "\r\n\r\n"), returnData(testData) {
        }
        virtual bool read(std::vector<char> & data, bool allowPartialRead, Host & hostState) {
            // good return on first call, 0 return on second
            UNUSED(allowPartialRead); UNUSED(hostState);
            counter += 1;
            switch (counter) {
                case 1:
                    data.resize(testString.length());
                    memcpy(&data[0], testString.c_str(), testString.length());
                    return true;
                case 2:
                    data.resize(returnData.size());
                    memcpy(&data[0], returnData.c_str(), returnData.length());
                    return true;
                case 3:
                    FAIL("Should only be called once");
                    return false;
            };
            FAIL("Should never get here");
            return false;
        };
        int counter = 0;
        const std::string testString;
        const std::string returnData;
    };
	std::unique_ptr<Protocol> mockProtocol(new MockProtocol(baseData, testData));
	RestHeaders headers;
    Host host = Host::ALL_INTERFACES4;
    std::vector<char> data;
    REQUIRE(headers.read(mockProtocol, data, host));
    REQUIRE(std::string(&data[0], data.size()) == expectedData);
}

TEST_CASE("RestHeaders: Read returns EOH, valid Content-Length and short content", "[headers]") {
    const std::string baseData = "This is a test\r\nSecond line\r\nContent-Length: ";
    const std::string testData = "0123456789testData";
    const std::string expectedData = baseData + std::to_string(testData.length()) +
    "\r\n\r\n" + testData;
    class MockProtocol : public Protocol {
    public:
        MockProtocol(const std::string & base, const std::string & testData) : counter(0),
        testString(base + std::to_string(testData.length()) +
                    "\r\n\r\n"), returnData(testData.substr(0, testData.length()/2)) {
        }
        virtual bool read(std::vector<char> & data, bool allowPartialRead, Host & hostState) {
            // good return on first call, 0 return on second
            UNUSED(allowPartialRead); UNUSED(hostState);
            counter += 1;
            switch (counter) {
                case 1:
                    data.resize(testString.length());
                    memcpy(&data[0], testString.c_str(), testString.length());
                    return true;
                case 2:
                    data.resize(returnData.size());
                    memcpy(&data[0], returnData.c_str(), returnData.length());
                    return true;
                case 3:
                    FAIL("Should only be called once");
                    return false;
            };
            FAIL("Should never get here");
            return false;
        };
        int counter = 0;
        const std::string testString;
        const std::string returnData;
    };
	std::unique_ptr<Protocol> mockProtocol(new MockProtocol(baseData, testData));
    RestHeaders headers;
    Host host = Host::ALL_INTERFACES4;
    std::vector<char> data;
    REQUIRE_FALSE(headers.read(mockProtocol, data, host));
}

TEST_CASE("RestHeaders: Read returns EOH, valid Content-Length and too much content", "[headers]") {
    const std::string baseData = "This is a test\r\nSecond line\r\nContent-Length: ";
    const std::string testData = "0123456789testData";
    const std::string expectedData = baseData + std::to_string(testData.length()) +
    "\r\n\r\n" + testData;
    class MockProtocol : public Protocol {
    public:
        MockProtocol(const std::string & base, const std::string & testData) : counter(0),
        testString(base + std::to_string(testData.length()) +
                    "\r\n\r\n"), returnData(testData) {
        }
        virtual bool read(std::vector<char> & data, bool allowPartialRead, Host & hostState) {
            // good return on first call, 0 return on second
            UNUSED(allowPartialRead); UNUSED(hostState);
            counter += 1;
            switch (counter) {
                case 1:
                    data.resize(testString.length());
                    memcpy(&data[0], testString.c_str(), testString.length());
                    return true;
                case 2:
                    data.resize(returnData.size() + 4);
                    memcpy(&data[0], returnData.c_str(), returnData.length());
                    memcpy(&data[returnData.size()], "ABCD", 4);
                    return true;
                case 3:
                    FAIL("Should only be called once");
                    return false;
            };
            FAIL("Should never get here");
            return false;
        };
        int counter = 0;
        const std::string testString;
        const std::string returnData;
    };
	std::unique_ptr<Protocol> mockProtocol(new MockProtocol(baseData, testData));
	RestHeaders headers;
    Host host = Host::ALL_INTERFACES4;
    std::vector<char> data;
    REQUIRE(headers.read(mockProtocol, data, host));
    REQUIRE(std::string(&data[0], data.size()) == expectedData);
}
