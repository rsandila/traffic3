#pragma once

// place holder class
class ContentManager {
public:
    ContentManager() {};
    virtual bool Stop() {
        return false;
    };
    virtual void setMinimumSize(unsigned size) noexcept {
    };
    virtual void setMaximumSize(unsigned size) noexcept {
    };
};