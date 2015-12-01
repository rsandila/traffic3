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

#include <cstdlib>
#include <cstring>
#include <cmath>
#include "restheaders.h"

RestHeaders::RestHeaders() {
    
}

RestHeaders::~RestHeaders() {
    
}


// TODO - consider chunked encoding at some stage
// TODO - consider HTTP/2 at some stage
bool RestHeaders::read(Protocol & protocol, std::vector<char> & content, Host & hostState) {
    std::vector<char> workBuffer, readBuffer;
    workBuffer.resize(0);
    uint32_t offset = 0;
    readBuffer.resize(10248);
    static const std::string CONTENT_LENGTH = "Content-Length:";
    static const std::string END_OF_HEADERS = "\r\n\r\n";
    uint32_t contentLength = (uint32_t)-1;
    uint32_t endOfHeaders = (uint32_t)-1;
    
    while(protocol.read(readBuffer, true, hostState)) {
        if (readBuffer.size() == 0) {
            break;
        }
        offset = workBuffer.size();
        workBuffer.resize(workBuffer.size() + readBuffer.size());
        memcpy(&workBuffer[offset], &readBuffer[0], readBuffer.size());
        if (endOfHeaders == (uint32_t)-1) {
            endOfHeaders = findStringInVector(workBuffer, 0, workBuffer.size(), END_OF_HEADERS);
        }
        if (endOfHeaders != (uint32_t)-1) {
            endOfHeaders += END_OF_HEADERS.length();
            uint32_t contentLengthOffset = findStringInVector(workBuffer, 0, endOfHeaders, CONTENT_LENGTH);
            if (contentLengthOffset != (uint32_t)-1) {
                contentLengthOffset += CONTENT_LENGTH.size();
                uint32_t maxNumber = workBuffer.size() - contentLengthOffset;
                if (maxNumber > 30) { // why 30... because
                    maxNumber = 30;
                }
                std::string toNumber(&workBuffer[contentLengthOffset], maxNumber);
                try {
                    int32_t signedContentLength = std::stol(toNumber);
                    contentLength = signedContentLength;
                    if (std::fabs(contentLength) != std::abs(signedContentLength)) {
                        return false;
                    }
                } catch (std::invalid_argument e) {
                    // invalid format
                    return false;
                }
                if (workBuffer.size() - endOfHeaders >= contentLength) {
                    break;
                }
                uint32_t expectedReadSize = contentLength - (workBuffer.size() - endOfHeaders);
                readBuffer.resize(expectedReadSize);
                if (protocol.read(readBuffer, false, hostState)) {
                    if (readBuffer.size() < expectedReadSize) {
                        return false;
                    }
                    uint32_t originalSize = workBuffer.size();
                    workBuffer.resize(workBuffer.size() + expectedReadSize);
                    memcpy(&workBuffer[originalSize], &readBuffer[0], expectedReadSize);
                    break;
                } else {
                    return false;
                }
            } else {
                // No content length and we have all the headers, so lets get out of here
                break;
            }
        }
    }
    content = std::move(workBuffer);
    return (content.size() != 0) && (endOfHeaders != (uint32_t)-1);
}

bool RestHeaders::write(Protocol & protocol, const std::vector<char> & content, const Host & hostState) {
    return protocol.write(content, hostState);
}

unsigned RestHeaders::getVersion() const {
    return 3;
}

uint32_t RestHeaders::findStringInVector(const std::vector<char> & buffer, uint32_t offset, uint32_t max_offset, const std::string & toFind) const
        noexcept {
    if (toFind.size() < 2 || buffer.size() - offset < toFind.size() || max_offset > buffer.size() ||
            max_offset < toFind.size()) {
        return (uint32_t)-1;
    }
    uint32_t endOfBuffer = max_offset - toFind.size();
    for (uint32_t counter = offset; counter <= endOfBuffer; counter++) {
        if (buffer[counter] == toFind[0] && buffer[counter + 1] == toFind[1]) {
            std::string toCompare(&buffer[counter], toFind.size());
            if (toCompare == toFind) {
                return counter;
            }
        }
    }
    return -1;
}
