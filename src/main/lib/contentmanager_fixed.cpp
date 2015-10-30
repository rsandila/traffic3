#include "contentmanager_fixed.h"

ContentManager_Fixed::ContentManager_Fixed(std::unique_ptr<Protocol> _protocol, CommonHeaders &_headerHandler, bool isServer) : ContentManagerBase(std::move(_protocol), _headerHandler, isServer) {
    
}
    
ContentManager_Fixed::~ContentManager_Fixed() {
    
}

ContentManagerType ContentManager_Fixed::getType() const noexcept {
    return ContentManagerType::Fixed;
}

std::vector<char> ContentManager_Fixed::ProcessContent(const std::vector<char> & incomingData) noexcept {
    return fixed;
}

bool ContentManager_Fixed::PrepareContent() noexcept {
    if (fixed.size() != getMax()) {
        fixed.resize(getMax());
        fixed.assign(fixed.size(), (char)getMin());
    }
    return true;
}

void ContentManager_Fixed::CleanupContent() noexcept {
}

void ContentManager_Fixed::setMaximumSize(unsigned size) noexcept {
    setMax(size);
}
