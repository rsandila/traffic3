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

#include <cstring>
#include <sstream>
#include "html_return.h"

std::vector<char> returnHtmlPage(unsigned code, const std::string message,  const std::string title,
                                 const std::string & body) {
    std::stringstream ostr;
    
    ostr << "HTTP/1.0 " << code << " " << message << "\r\nConnection: close\r\nContent-Length: ";
    std::stringstream returnBody;
    returnBody << "<!DOCTYPE HTML PUBLIC ""-//IETF//DTD HTML 2.0//EN"">\r\n<html><head><title>";
    returnBody << title << "</title></head><body><h1>" << body << "</h1>";
    returnBody << "</body></html>\r\n";
    ostr << returnBody.str().length() << "\r\n\r\n" << returnBody.str();
    std::vector<char> returnValue(ostr.str().length());
    memcpy(&returnValue[0], ostr.str().c_str(), returnValue.size());
    return returnValue;
}

std::vector<char> returnJsonPage(unsigned code, const std::string message, const std::string & dump) {
    std::stringstream ostr;
    
    ostr << "HTTP/1.1 " << code << " " << message << "\r\nConnection: open\r\nContent-Length: ";
    ostr << dump.length() << "\r\n\r\n" << dump;

    std::vector<char> returnValue(ostr.str().length());
    memcpy(&returnValue[0], ostr.str().c_str(), returnValue.size());
    return returnValue;
}
