#pragma once

#include "host.h"
#include "protocol.h"
#include "contentmanager.h"
#include "contentmanager_random_text.h"
#include "contentmanagertype.h"

class ContentManagerFactory {
public:
    ContentManagerFactory(const ContentManagerType & _type, unsigned _min, unsigned _max, CommonHeaders _headerHandler) : type(_type),
            min(_min), max(_max), headerHandler(_headerHandler) {
    };
    virtual std::unique_ptr<ContentManager> createContentManager(std::unique_ptr<Protocol> protocol) {
        switch (type) {
            case ContentManagerType::RandomText:
                return withCustomizations(std::unique_ptr<ContentManager>(new ContentManager_Random_Text(std::move(protocol), headerHandler)));
             default:
                return std::unique_ptr<ContentManager>(new ContentManager());
        }
    };
protected:
    virtual std::unique_ptr<ContentManager> withCustomizations(std::unique_ptr<ContentManager> contentManager) const {
        if (contentManager.get() != nullptr) {
            contentManager->setMinimumSize(min);
            contentManager->setMaximumSize(max);
        }
        return std::move(contentManager);
    }
private:
    unsigned min, max;
    ContentManagerType type;
    CommonHeaders headerHandler;
};