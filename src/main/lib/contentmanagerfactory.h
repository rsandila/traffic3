#pragma once

#include "host.h"
#include "protocol.h"
#include "contentmanager.h"
#include "contentmanager_random_text.h"
#include "contentmanagertype.h"

// place holder class
class ContentManagerFactory {
public:
    ContentManagerFactory(const ContentManagerType & _type) : type(_type) {
    };
    virtual ContentManager * createContentManager(Protocol & protocol) {
        switch (type) {
            case ContentManagerType::RandomText:
                return new ContentManager_Random_Text(protocol);
             default:
                return new ContentManager();
        }
    };
private:
    ContentManagerType type;
};