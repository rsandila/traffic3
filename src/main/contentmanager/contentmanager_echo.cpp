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

#include "contentmanager_echo.h"
#include "lib/common.h"
#include "lib/logging.h"

ContentManager_Echo::ContentManager_Echo(std::unique_ptr<Protocol> _protocol, CommonHeaders &_headerHandler, bool isServer) : ContentManagerBase(std::move(_protocol), _headerHandler, isServer) {
    
}

ContentManager_Echo::~ContentManager_Echo() {
    
}

ContentManagerType ContentManager_Echo::getType() const noexcept {
    return ContentManagerType::Echo;
}

std::vector<char> ContentManager_Echo::ProcessContent(const std::vector<char> & incomingData) noexcept {
    return incomingData;
}

bool ContentManager_Echo::PrepareContent() noexcept {
    return true;
}

void ContentManager_Echo::CleanupContent() noexcept {
}

void ContentManager_Echo::setMaximumSize(unsigned long size) noexcept {
    setMax(size);
}
