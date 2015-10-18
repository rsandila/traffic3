#pragma once

#include "host.h"
#include "protocol.h"
#include "contentmanager.h"
#include "contentmanager_random_text.h"
#include "contentmanagertype.h"

class ContentManagerFactory {
public:
    ContentManagerFactory(const ContentManagerType & _type) : type(_type) {
    };
    virtual std::unique_ptr<ContentManager> createContentManager(std::unique_ptr<Protocol> protocol) {
        switch (type) {
            case ContentManagerType::RandomText:
                return std::unique_ptr<ContentManager>(new ContentManager_Random_Text(std::move(protocol)));
             default:
                return std::unique_ptr<ContentManager>(new ContentManager());
        }
    };
private:
    ContentManagerType type;
};