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

#include "contentmanager_fixed.h"
#include "lib/common.h"
#include "lib/logging.h"

ContentManager_Fixed::ContentManager_Fixed(std::unique_ptr<Protocol> _protocol, CommonHeaders &_headerHandler, bool isServer) : ContentManagerBase(std::move(_protocol), _headerHandler, isServer) {
    
}
    
ContentManager_Fixed::~ContentManager_Fixed() {
    
}

ContentManagerType ContentManager_Fixed::getType() const noexcept {
    return ContentManagerType::Fixed;
}

std::vector<char> ContentManager_Fixed::ProcessContent(const std::vector<char> & incomingData) noexcept {
    UNUSED(incomingData);
    LOG(DEBUG) << "entering with " << incomingData.size() << std::endl;
    LOG(DEBUG) << "existing with " << fixed.size() << std::endl;
    return fixed;
}

bool ContentManager_Fixed::PrepareContent() noexcept {
    if (fixed.size() != getMax()) {
        fixed.resize(getMax());
        fixed.assign(fixed.size(), (char)getMin());
    }
    return true;
}

void ContentManager_Fixed::CleanupContent() noexcept {
}

void ContentManager_Fixed::setMaximumSize(unsigned long size) noexcept {
    setMax(size);
}
