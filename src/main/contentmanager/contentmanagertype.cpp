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
#include <algorithm>
#include <cctype>
#include <map>
#include "contentmanagertype.h"

static std::map<std::string, ContentManagerType> contentManagerMap {
    {"randomtext", ContentManagerType::RandomText},
    {"randombinary", ContentManagerType::RandomBinary},
    {"fixed", ContentManagerType::Fixed},
    {"echo", ContentManagerType::Echo}
};

ContentManagerType convertStringToContentManagerType(const std::string & name) {
    std::string lname = name;
    std::for_each(name.begin(), name.end(), [](char a){ return std::tolower(a);});
    if (contentManagerMap.find(lname) == contentManagerMap.end()) {
        throw std::invalid_argument("Invalid ContentManagerType");
    }
    return contentManagerMap[lname];
}

const std::string convertContentManagerTypeToString(const ContentManagerType type) {
    for (const auto & it: contentManagerMap) {
        if (it.second == type) {
            return it.first;
        }
    }
    return "none";
}

nlohmann::json contentManagerTypesToJson() {
    nlohmann::json returnValue;
    
    // Not returning None and Rest as they are special cases
    returnValue[convertContentManagerTypeToString(ContentManagerType::Echo)] = "Echo incoming data back";
    returnValue[convertContentManagerTypeToString(ContentManagerType::Fixed)] = "Return a fixed buffer";
    returnValue[convertContentManagerTypeToString(ContentManagerType::RandomBinary)] = "Return random binary data";
    returnValue[convertContentManagerTypeToString(ContentManagerType::RandomText)] = "Return random text data";
    
    return returnValue;
}
