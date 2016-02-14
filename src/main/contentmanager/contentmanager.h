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
#pragma once

#include "contentmanagertype.h"
#include "lib/common.h"
#include "json.hpp"

// place holder class, don't derive from this one, derive from ContentManagerBase
class ContentManager {
public:
    ContentManager() {};
    ContentManager(ContentManager && other) {
        UNUSED(other);
    };
    ContentManager & operator=(ContentManager&& other) {
        UNUSED(other);
        return *this;
    }
    virtual ~ContentManager() {};
    virtual bool Stop() {
        return false;
    };
    virtual bool Start() {
        return false;
    }
    virtual void setMinimumSize(unsigned long size) noexcept {
        UNUSED(size);
    };
    virtual void setMaximumSize(unsigned long size) noexcept {
        UNUSED(size);
    };
    virtual ContentManagerType getType() const noexcept {
        return ContentManagerType::None;
    }
    virtual long long getBytesRead() const noexcept {
        return 0LL;
    }
    virtual long long getBytesWritten() const noexcept {
        return 0LL;
    }
    virtual nlohmann::json toJson() const noexcept {
        return nlohmann::json();
    }
private:
    ContentManager(const ContentManager &) = delete;
    ContentManager & operator=(const ContentManager &) = delete;
};

