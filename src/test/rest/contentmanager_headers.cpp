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
#include "hippomocks.h"
#include "rest/contentmanager_headers.h"
#include "rest/restheaders.h"

TEST_CASE("ContentManager_Headers", "[rest][content][server]") {
    SECTION("Different request types") {
        RestHeaders restHeaders;
        std::unique_ptr<Protocol> protocol(new Protocol("Mock"));
        
        class ContentManager_Headers_Test: public ContentManager_Headers {
        public:
            ContentManager_Headers_Test(std::unique_ptr<Protocol> _protocol, CommonHeaders &_headerHandler,
                                        bool isServer) : ContentManager_Headers(std::move(_protocol), _headerHandler, isServer) {
            }
            bool RunTest1() {
                MockRepository mocks;

                std::vector<char> errorReturnValue { 'a' };
                std::vector<char> goodReturnValue { 'b' };
                
                ErrorPageHandler * errorPageHandlerTemp = mocks.Mock<ErrorPageHandler>();
                
                std::vector<std::shared_ptr<ErrorPageHandler>> errorHandler { std::shared_ptr<ErrorPageHandler>(errorPageHandlerTemp) };
                addErrorPageHandlers(errorHandler);
                
                mocks.ExpectCall(errorPageHandlerTemp, ErrorPageHandler::handleError).Do([](ErrorTypes type, const std::vector<char> & incomingData) -> std::vector<char> { UNUSED(type); UNUSED(incomingData); return std::vector<char> { 'a' };});

                REQUIRE(ContentManager_Headers::ProcessContent(std::vector<char>(), Host::ALL_INTERFACES4) == errorReturnValue);
                
                mocks.ExpectCall(errorPageHandlerTemp, ErrorPageHandler::handleError).Do([](ErrorTypes type, const std::vector<char> & incomingData) -> std::vector<char> { UNUSED(type); UNUSED(incomingData); return std::vector<char> { 'a' };});
                REQUIRE(ContentManager_Headers::ProcessContent(errorReturnValue, Host::ALL_INTERFACES4) == errorReturnValue);
                std::vector<char> singleBadLine { 'a', '\r', '\n' };
                mocks.ExpectCall(errorPageHandlerTemp, ErrorPageHandler::handleError).Do([](ErrorTypes type, const std::vector<char> & incomingData) -> std::vector<char> { UNUSED(type); UNUSED(incomingData); return std::vector<char> { 'a' };});
                
                REQUIRE(ContentManager_Headers::ProcessContent(singleBadLine, Host::ALL_INTERFACES4) == errorReturnValue);
                std::vector<char> secondBadLine { 'B', 'O', 'O', ' ', '/', ' ', 'H', 'T', 'T', 'P', '/', '1', '.', '0', '\r', '\n' };
                mocks.ExpectCall(errorPageHandlerTemp, ErrorPageHandler::handleError).Do([](ErrorTypes type, const std::vector<char> & incomingData) -> std::vector<char> { UNUSED(type); UNUSED(incomingData); return std::vector<char> { 'a' };});
                
                REQUIRE(ContentManager_Headers::ProcessContent(secondBadLine, Host::ALL_INTERFACES4) == errorReturnValue);
                // test with bad header
                std::vector<char> thirdBadLine { 'G', 'E', 'T', ' ', '/', ' ', 'H', 'T', 'T', 'P', '/', '1', '.', '0', '\r', '\n', 'A', '\r', '\n' };
                mocks.ExpectCall(errorPageHandlerTemp, ErrorPageHandler::handleError).Do([](ErrorTypes type, const std::vector<char> & incomingData) -> std::vector<char> { UNUSED(type); UNUSED(incomingData); return std::vector<char> { 'a' };});
                
                REQUIRE(ContentManager_Headers::ProcessContent(secondBadLine, Host::ALL_INTERFACES4) == errorReturnValue);
                // test with good header
                // test with no body
                std::vector<char> goodLineNoBody { 'G', 'E', 'T', ' ', '/', ' ', 'H', 'T', 'T', 'P', '/', '1', '.', '0', '\r', '\n', 'A', ':', 'B', '\r', '\n',
                    '\r', '\n' };
                mocks.ExpectCall(errorPageHandlerTemp, ErrorPageHandler::handleError).Do([](ErrorTypes type, const std::vector<char> & incomingData) -> std::vector<char> { UNUSED(type); UNUSED(incomingData); return std::vector<char> { 'a' };});
                // Expects error if no rest handlers defined
                REQUIRE(ContentManager_Headers::ProcessContent(goodLineNoBody, Host::ALL_INTERFACES4) == errorReturnValue);

                RestRequestHandler * restRequestHandlerTemp = mocks.Mock<RestRequestHandler>();
                std::vector<std::shared_ptr<RestRequestHandler>> restHandler { std::shared_ptr<RestRequestHandler>(restRequestHandlerTemp) };
                addRestRequestHandlers(restHandler);
                mocks.NeverCall(errorPageHandlerTemp, ErrorPageHandler::handleError);
                mocks.ExpectCall(restRequestHandlerTemp, RestRequestHandler::handleRequest).Do(
                            [](const Host & host, const RestRequest & request, const std::map<std::string, std::string> & headers, const std::vector<char> & body) -> std::vector<char> {
                                    UNUSED(host);
                                REQUIRE(request.getType() == RestRequestType::RRT_GET);
                                REQUIRE(request.getVersion() == "HTTP/1.0");
                                REQUIRE(request.getUri() == "/");
                                REQUIRE(headers.size() == 1);
                                REQUIRE(headers.find("A") != headers.end());
                                REQUIRE(headers.at("A") == "B");
                                REQUIRE(body.size() == 0);
                                return std::vector<char> { 'b' };
                            });
                REQUIRE(ContentManager_Headers::ProcessContent(goodLineNoBody, Host::ALL_INTERFACES4) == goodReturnValue);

                // test with body
                std::vector<char> goodLineWithBody { 'G', 'E', 'T', ' ', '/', ' ', 'H', 'T', 'T', 'P', '/', '1', '.', '0', '\r', '\n', 'A', ':', 'B', '\r', '\n',
                    '\r', '\n', 'c', 'd' };
                mocks.NeverCall(errorPageHandlerTemp, ErrorPageHandler::handleError);
                mocks.ExpectCall(restRequestHandlerTemp, RestRequestHandler::handleRequest).Do(
                                                                                               [](const Host & host, const RestRequest & request, const std::map<std::string, std::string> & headers, const std::vector<char> & body) -> std::vector<char> {
                                                                                                   UNUSED(host);
                                                                                                   REQUIRE(request.getType() == RestRequestType::RRT_GET);
                                                                                                   REQUIRE(request.getVersion() == "HTTP/1.0");
                                                                                                   REQUIRE(request.getUri() == "/");
                                                                                                   REQUIRE(headers.size() == 1);
                                                                                                   REQUIRE(headers.find("A") != headers.end());
                                                                                                   REQUIRE(headers.at("A") == "B");
                                                                                                   REQUIRE(body.size() == 2);
                                                                                                   REQUIRE(body == (std::vector<char> { 'c', 'd' }));
                                                                                                   return std::vector<char> { 'b' };
                                                                                               });
                REQUIRE(ContentManager_Headers::ProcessContent(goodLineWithBody, Host::ALL_INTERFACES4) == goodReturnValue);
                
                
                mocks.ExpectCallDestructor(errorHandler[0].get());
                errorHandler[0].reset();
                errorHandlers[0].reset();
                mocks.ExpectCallDestructor(restHandler[0].get());
                restHandlers[0].reset();
                restHandler[0].reset();

                return true;
            }
        protected:
            virtual std::vector<char> ProcessContent(const std::vector<char> & incomingData, const Host & host) noexcept override {
                return ContentManager_Headers::ProcessContent(incomingData, host);
            }
        };
        
        ContentManager_Headers_Test contentManagerHeaders(std::move(protocol), restHeaders, true);
                std::vector<std::shared_ptr<RestRequestHandler>> restHandler;
        contentManagerHeaders.addRestRequestHandlers(restHandler);
        
        REQUIRE(contentManagerHeaders.RunTest1());
    }
}
