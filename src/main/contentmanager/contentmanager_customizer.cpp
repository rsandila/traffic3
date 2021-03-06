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

#include "contentmanager_customizer.h"


ContentManagerCustomizer::ContentManagerCustomizer(unsigned minimum, unsigned maximum)
        : _minimum(minimum), _maximum(maximum) {
}

std::unique_ptr<ContentManager> ContentManagerCustomizer::customize(std::unique_ptr<ContentManager> contentManager) {
    contentManager->setMinimumSize(_minimum);
    contentManager->setMaximumSize(_maximum);
    return contentManager;
}

nlohmann::json ContentManagerCustomizer::toJson() const noexcept {
    nlohmann::json returnValue;
    
    returnValue["min"] = _minimum;
    returnValue["max"] = _maximum;
    returnValue["type"] = "ContentManagerCustomizer";
    
    return returnValue;
}