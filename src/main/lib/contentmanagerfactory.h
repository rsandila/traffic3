#pragma once

#include "host.h"
#include "protocol.h"
#include "contentmanager.h"
#include "contentmanager_random_text.h"

// place holder class
class ContentManagerFactory {
public:
    enum class Type {
        RandomText
    };
    ContentManagerFactory(const Type & _type) : type(_type) {
    };
    virtual ContentManager createContentManager(Protocol & protocol) {
        switch (type) {
            case Type::RandomText:
                return std::move(ContentManager_Random_Text(protocol));
            default:
                return std::move(ContentManager());
        }
    };
private:
    Type type;
};