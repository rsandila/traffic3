#pragma once

#include <thread>
#include <random>
#include "contentmanager.h"
#include "protocol.h"

// place holder class
class ContentManager_Random_Text : public ContentManager {
public:
    ContentManager_Random_Text(std::unique_ptr<Protocol> _protocol);
    ContentManager_Random_Text(ContentManager_Random_Text && other);
    ContentManager_Random_Text & operator=(ContentManager_Random_Text&& other);
    virtual ~ContentManager_Random_Text();
    virtual bool Stop() override;
    virtual bool Start() override;
    virtual void setMinimumSize(unsigned size) noexcept override ;
    virtual void setMaximumSize(unsigned size) noexcept override;
    virtual ContentManagerType getType() const noexcept override;
protected:
    void Worker();
private:
    std::unique_ptr<Protocol> protocol;
    std::thread worker;
    unsigned min, max;
    std::atomic<bool> started;
    std::atomic<bool> running;
    std::atomic<bool> doExitBeforeStart;
    ContentManager_Random_Text(const ContentManager_Random_Text &) = delete;
    ContentManager_Random_Text & operator=(const ContentManager_Random_Text &) = delete;
};