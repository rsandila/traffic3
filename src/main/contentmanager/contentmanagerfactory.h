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

#include "lib/host.h"
#include "protocol/protocol.h"
#include "contentmanager/contentmanager.h"
#include "contentmanager/contentmanager_random_text.h"
#include "contentmanager/contentmanager_fixed.h"
#include "contentmanager/contentmanager_echo.h"
#include "contentmanager/contentmanager_random_binary.h"
#include "contentmanager/contentmanagertype.h"
#include "rest/contentmanager_headers.h"

class ContentManagerFactory {
public:
    ContentManagerFactory(const ContentManagerType & _type, unsigned _min, unsigned _max, CommonHeaders & _headerHandler) :
            minimum(_min), maximum(_max), type(_type), headerHandler(_headerHandler) {
    };
    virtual ~ContentManagerFactory() {;};
    virtual std::unique_ptr<ContentManager> createContentManager(std::unique_ptr<Protocol> protocol, bool isServer) {
        switch (type) {
            case ContentManagerType::RandomText:
                return withCustomizations(std::unique_ptr<ContentManager>(new ContentManager_Random_Text(std::move(protocol), headerHandler, isServer)));
            case ContentManagerType::RandomBinary:
                return withCustomizations(std::unique_ptr<ContentManager>(new ContentManager_Random_Binary(std::move(protocol), headerHandler, isServer)));
            case ContentManagerType::Fixed:
                return withCustomizations(std::unique_ptr<ContentManager>(new ContentManager_Fixed(std::move(protocol), headerHandler, isServer)));
            case ContentManagerType::Echo:
                return withCustomizations(std::unique_ptr<ContentManager>(new ContentManager_Echo(std::move(protocol), headerHandler, isServer)));
            case ContentManagerType::RestHeaders:
                return withCustomizations(std::unique_ptr<ContentManager>(new ContentManager_Headers(std::move(protocol), headerHandler, isServer)));
            default:
                return std::unique_ptr<ContentManager>(new ContentManager());
        }
    };
    ContentManagerFactory(const ContentManagerFactory & other) : minimum(other.minimum), maximum(other.maximum),
            type(other.type), headerHandler(other.headerHandler) {
    }
    ContentManagerFactory & operator=(const ContentManagerFactory & other) {
        minimum = other.minimum;
        maximum = other.maximum;
        type = other.type;
        headerHandler = other.headerHandler;
        return *this;
    }
protected:
    virtual std::unique_ptr<ContentManager> withCustomizations(std::unique_ptr<ContentManager> contentManager) const {
        if (contentManager.get() != nullptr) {
            contentManager->setMinimumSize(minimum);
            contentManager->setMaximumSize(maximum);
        }
        return std::move(contentManager);
    }
private:
    unsigned minimum, maximum;
    ContentManagerType type;
    CommonHeaders & headerHandler;
};
