#pragma once

#include "host.h"
#include "protocol.h"
#include "contentmanager.h"

// place holder class
class ContentManagerFactory {
public:
    ContentManagerFactory() {
    };
    virtual ContentManager createContentManager(const Host & local, int local_port) {
        return std::move(ContentManager());
    };
};