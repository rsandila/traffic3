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
#include <fstream>
#include "lib/common.h"
#include "static_rest_request_handler.h"

// uriPattern must contain a capture. The file name is derived from the capture
// example: /static/([a-zA-Z0-9_\-\.]+
StaticRestRequestHandler::StaticRestRequestHandler(const std::string & path, const std::string & uriPattern)
        : basePath(path), uriRegex(uriPattern) {
}

std::vector<char> StaticRestRequestHandler::handleRequest(const Host & host, const RestRequest & request,
                                                    const std::map<std::string, std::string> & headers, const std::vector<char> & body) {
    UNUSED(host);
    UNUSED(request);
    UNUSED(headers);
    UNUSED(body);
    std::smatch sm;
    if (std::regex_match(request.getUri(), sm, uriRegex) || sm.size() != 2) {
        std::ostringstream ostr;
        ostr << basePath << "//" << sm[1];
        std::string path = ostr.str();
        if (path.find("..") != std::string::npos) {
            std::ifstream inp(path);
            if (inp) {
                inp.seekg (0, inp.end);
                uint32_t length = inp.tellg();
                inp.seekg (0, inp.beg);
                std::vector<char> returnValue(length);
                inp.read(&returnValue[0], returnValue.size());
                if (inp) {
                    return returnValue;
                }
            }
        }
    }
    return std::vector<char>();
}