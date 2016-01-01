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

#include "rest/rest_status.h"
#include "rest/rest_state.h"

RestStatus::RestStatus(const std::string & uriPattern, RestState & _state) : state(_state) {
    // TODO
}

std::vector<char> RestStatus::handleRequest(const Host & host, const RestRequest & request, const std::map<std::string, std::string> & headers, const std::vector<char> & body) {
    // TODO - get info from state and report it as a html/json response
    return std::vector<char>();
}