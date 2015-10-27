#pragma once

#pragma once

#include <thread>
#include <random>
#include "contentmanager.h"
#include "protocol.h"
#include "commonheaders.h"

// place holder class, derive new ContentManagers from this class
class ContentManagerBase : public ContentManager {
public:
    ContentManagerBase(std::unique_ptr<Protocol> _protocol, CommonHeaders &_headerHandler);
    ContentManagerBase(ContentManagerBase && other);
    ContentManagerBase & operator=(ContentManagerBase&& other);
    virtual ~ContentManagerBase();
    virtual bool Stop() noexcept override;
    virtual bool Start() noexcept override;
    virtual void setMinimumSize(unsigned size) noexcept override;
    virtual void setMaximumSize(unsigned size) noexcept override;
    virtual ContentManagerType getType() const noexcept override = 0;
protected:
    void Worker() noexcept;
    virtual std::vector<char> ProcessContent(const std::vector<char> & incomingData) noexcept = 0;
    virtual bool PrepareContent() noexcept = 0;
    virtual void CleanupContent() noexcept = 0;
    int getMin() const noexcept {
        return min;
    }
    int getMax() const noexcept {
        return max;
    }
private:
    std::unique_ptr<Protocol> protocol;
    std::thread worker;
    unsigned min, max;
    std::atomic<bool> started;
    std::atomic<bool> running;
    std::atomic<bool> doExitBeforeStart;
    CommonHeaders & headerHandler;
    
    ContentManagerBase(const ContentManagerBase &) = delete;
    ContentManagerBase & operator=(const ContentManagerBase &) = delete;
};