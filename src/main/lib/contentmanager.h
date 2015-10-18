#pragma once

#include "contentmanagertype.h"

// place holder class
class ContentManager {
public:
    ContentManager() {};
    ContentManager(ContentManager && other) {
    };
    ContentManager & operator=(ContentManager&& other) {
        return *this;
    }
    virtual bool Stop() {
        return false;
    };
    virtual bool Start() {
        return false;
    }
    virtual void setMinimumSize(unsigned size) noexcept {
    };
    virtual void setMaximumSize(unsigned size) noexcept {
    };
    virtual ContentManagerType getType() const noexcept {
        return ContentManagerType::None;
    }
private:
    ContentManager(const ContentManager &) = delete;
    ContentManager & operator=(const ContentManager &) = delete;
};