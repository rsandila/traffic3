#pragma once

#include <thread>
#include <random>
#include "contentmanager_base.h"
#include "protocol.h"

// place holder class
class ContentManager_Random_Text : public ContentManagerBase {
public:
    ContentManager_Random_Text(std::unique_ptr<Protocol> _protocol, CommonHeaders &_headerHandler);
    virtual ~ContentManager_Random_Text();
    virtual ContentManagerType getType() const noexcept override;
protected:
    virtual std::vector<char> ProcessContent(const std::vector<char> & incomingData) noexcept override;
    virtual bool PrepareContent() noexcept override;
    virtual void CleanupContent() noexcept override;
private:
    std::default_random_engine generator;
    std::uniform_int_distribution<char> chars;
    std::unique_ptr<std::uniform_int_distribution<int>> distribution;
};