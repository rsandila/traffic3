#pragma once

#include <thread>
#include <random>
#include "contentmanager_base.h"
#include "protocol.h"

// place holder class
class ContentManager_Fixed : public ContentManagerBase {
public:
    ContentManager_Fixed(std::unique_ptr<Protocol> _protocol, CommonHeaders &_headerHandler, bool isServer);
    virtual ~ContentManager_Fixed();
    virtual ContentManagerType getType() const noexcept override;
    void setMaximumSize(unsigned size) noexcept override;
protected:
    virtual std::vector<char> ProcessContent(const std::vector<char> & incomingData) noexcept override;
    virtual bool PrepareContent() noexcept override;
    virtual void CleanupContent() noexcept override;
private:
    std::vector<char> fixed;
};