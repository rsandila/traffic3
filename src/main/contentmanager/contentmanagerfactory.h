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
#include "contentmanager/contentmanager_customizer.h"
#include "rest/contentmanager_headers.h"
#include "json.hpp"

class ContentManagerFactory {
public:
    ContentManagerFactory(const ContentManagerType & _type, std::unique_ptr<CommonHeaders> & _headerHandler,
                          std::shared_ptr<ContentManagerCustomizer> & customizer)
            : type(_type), headerHandler(std::move(_headerHandler)), customizerHandler(std::move(customizer)) {
    };
    virtual ~ContentManagerFactory() {;};
    virtual std::unique_ptr<ContentManager> createContentManager(std::unique_ptr<Protocol> protocol, bool isServer) {
        switch (type) {
            case ContentManagerType::RandomText:
                return withCustomizations(std::unique_ptr<ContentManager>(new ContentManager_Random_Text(std::move(protocol), *headerHandler, isServer)));
            case ContentManagerType::RandomBinary:
                return withCustomizations(std::unique_ptr<ContentManager>(new ContentManager_Random_Binary(std::move(protocol), *headerHandler, isServer)));
            case ContentManagerType::Fixed:
                return withCustomizations(std::unique_ptr<ContentManager>(new ContentManager_Fixed(std::move(protocol),
                       *headerHandler, isServer)));
            case ContentManagerType::Echo:
                return withCustomizations(std::unique_ptr<ContentManager>(new ContentManager_Echo(std::move(protocol),
                        *headerHandler, isServer)));
            case ContentManagerType::RestHeaders:
                return withCustomizations(std::unique_ptr<ContentManager>(new ContentManager_Headers(std::move(protocol),
                        *headerHandler, isServer)));
            default:
                return std::unique_ptr<ContentManager>(new ContentManager());
        }
    };
    
    virtual nlohmann::json toJson() const noexcept {
        nlohmann::json returnValue;
        
        returnValue["headerHandler"] = headerHandler->toJson();
        returnValue["customizer"] = customizerHandler->toJson();
        returnValue["type"] = convertContentManagerTypeToString(type);
        
        return std::move(returnValue);
    }
protected:
    virtual std::unique_ptr<ContentManager> withCustomizations(std::unique_ptr<ContentManager> contentManager) const {
        if (contentManager.get() != nullptr && customizerHandler.get() != nullptr) {
            return std::move(customizerHandler->customize(std::move(contentManager)));
        }
        return std::move(contentManager);
    }
private:
    ContentManagerType type;
    std::unique_ptr<CommonHeaders> headerHandler;
    std::shared_ptr<ContentManagerCustomizer> customizerHandler;
};
