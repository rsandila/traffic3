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
#include <map>
#include <algorithm>
#include <string>
#include <iostream>
#include <sstream>
#include "rest_request_type.h"
#include "rest_request.h"
#include "contentmanager_headers.h"

static const std::map<std::string, RestRequestType> REST_REQUEST_TYPE_NAMES {
    {"GET", RestRequestType::RRT_GET},
    {"PUT", RestRequestType::RRT_PUT},
    {"POST", RestRequestType::RRT_POST},
    {"HEAD", RestRequestType::RRT_HEAD},
    {"DELETE", RestRequestType::RRT_DELETE},
    {"TRACE", RestRequestType::RRT_TRACE},
    {"OPTIONS", RestRequestType::RRT_OPTIONS},
    {"CONNECT", RestRequestType::RRT_CONNECT}
};

ContentManager_Headers::ContentManager_Headers(std::unique_ptr<Protocol> _protocol, std::shared_ptr<CommonHeaders> &_headerHandler,
                                               bool isServer) : ContentManagerBase(std::move(_protocol), _headerHandler, isServer) {
}

ContentManager_Headers::~ContentManager_Headers() {
}

ContentManagerType ContentManager_Headers::getType() const noexcept {
    return ContentManagerType::RestHeaders;
}

std::vector<char> ContentManager_Headers::ProcessContent(const std::vector<char> & incomingData, const Host & host) noexcept {
    unsigned int offset = 0;
    std::string requestLine = getLineFromVector(incomingData, offset);
    if (requestLine.empty()) {
        return returnErrorPage(ErrorTypes::EmptyRequest, incomingData);
    }
    // split request line out
    trimString(requestLine);
    std::stringstream ss(requestLine);
    std::string item;
    std::vector<std::string> fields;
    while (std::getline(ss, item, ' ')) {
        fields.push_back(item);
    }
    if (fields.size() != 3) {
        return returnErrorPage(ErrorTypes::BadlyFormedRequest, incomingData);
    }
    std::transform(fields[0].begin(), fields[0].end(), fields[0].begin(), toupper);
    if (REST_REQUEST_TYPE_NAMES.find(fields[0]) == REST_REQUEST_TYPE_NAMES.end()) {
        return returnErrorPage(ErrorTypes::UnknownRequestType, incomingData);
    }
    RestRequest request(REST_REQUEST_TYPE_NAMES.find(fields[0])->second, fields[1], fields[2]);
    // split headers out into a map
    std::map<std::string, std::string> headers;
    for (;;) {
        std::string line = getLineFromVector(incomingData, offset);
        if (line.empty()) {
            break;
        }
        size_t pos = line.find(":");
        if (pos == std::string::npos) {
            // TODO - what about headers with continuations
            return returnErrorPage(ErrorTypes::BadlyFormatedHeader, incomingData);
        }
        std::string name = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        trimString(name);
        trimString(value);
        headers[name] = value;
    }
    // split body out
    std::vector<char> body(incomingData.size() - offset);
    std::copy(incomingData.begin() + offset, incomingData.end(), body.begin());
    // call possible REST responders
    for (auto & handler: restHandlers) {
        std::vector<char> resultValue = handler->handleRequest(host, request, headers, body);
        if (!resultValue.empty()) {
            return resultValue;
        }
    }
    return returnErrorPage(ErrorTypes::NotFound, incomingData);
}

bool ContentManager_Headers::PrepareContent() noexcept {
    return true;
}

void ContentManager_Headers::CleanupContent() noexcept {
}

bool ContentManager_Headers::addErrorPageHandlers(std::vector<std::shared_ptr<ErrorPageHandler>> & errorHandler) {
    errorHandlers.insert(errorHandlers.begin(), errorHandler.begin(), errorHandler.end());
    return true;
}

bool ContentManager_Headers::addRestRequestHandlers(std::vector<std::shared_ptr<RestRequestHandler>> & restHandler) {
    restHandlers.insert(restHandlers.begin(), restHandler.begin(), restHandler.end());
    return true;
}

std::string ContentManager_Headers::getLineFromVector(const std::vector<char> & data, unsigned int & startOffset) noexcept {
    std::ostringstream ostr;
    
    while (startOffset < data.size() && data[startOffset] != '\r') {
        ostr << data[startOffset];
        startOffset++;
    }
    if ((startOffset + 1) < data.size() && data[startOffset] == '\r' && data[startOffset + 1] == '\n') {
        startOffset += 2;
        return ostr.str();
    } else {
        return "";
    }
}
        
std::vector<char> ContentManager_Headers::returnErrorPage(ErrorTypes type, const std::vector<char> & incomingData) {
    // walk through errorHandlers and return the first one that returns a non-empty vector
    for (auto & handler: errorHandlers) {
        if (handler.get()) {
            std::vector<char> retVal = handler->handleError(type, incomingData);
            if (!retVal.empty()) {
                return retVal;
            }
        }
    }
    return std::vector<char>();
}
        
void ContentManager_Headers::trimString(std::string & line) const noexcept {
    const std::string whitespaces(" \t\f\v\n\r");
    std::size_t found = line.find_last_not_of(whitespaces);
    if (found != std::string::npos) {
        line.erase(found + 1);
    }
    found = line.find_first_not_of(whitespaces);
    if (found != std::string::npos) {
        line = line.substr(found);
    }
}
