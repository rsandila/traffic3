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
#include <sstream>
#include <cstring>
#include "lib/common.h"
#include "errorpage_handler.h"

std::vector<char> ErrorPageHandler::handleError(ErrorTypes type, const std::vector<char> & incomingData) const noexcept {
    UNUSED(type);
    UNUSED(incomingData);
    std::stringstream ostr;
    std::string message;
    unsigned code;
    switch (type) {
        case ErrorTypes::EmptyRequest:
            code = 400;
            message = "Empty Request";
            break;
        case ErrorTypes::BadlyFormedRequest:
            code = 400;
            message = "Badly formed request line";
            break;
        case ErrorTypes::UnknownRequestType:
            code = 400;
            message = "Unknown type of request";
            break;
        case ErrorTypes::BadlyFormatedHeader:
            code = 400;
            message = "Badly formatted header";
            break;
        case ErrorTypes::NotFound:
            code = 404;
            message = "Resource not found";
            break;
            
    }
    ostr << "HTTP/1.0 " << code << message << "\r\n\r\n";
    std::vector<char> returnValue(ostr.str().length());
    memcpy(&returnValue[0], ostr.str().c_str(), returnValue.size());
    return std::move(returnValue);
}
