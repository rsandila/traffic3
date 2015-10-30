#include <thread>
#include <random>
#include "contentmanager_random_text.h"

ContentManager_Random_Text::ContentManager_Random_Text(std::unique_ptr<Protocol> _protocol, CommonHeaders &_headerHandler, bool isServer) :
        generator(std::chrono::system_clock::now().time_since_epoch().count()),
        chars(32, 127), distribution(nullptr),
        ContentManagerBase(std::move(_protocol), _headerHandler, isServer) {
}

ContentManager_Random_Text::~ContentManager_Random_Text() {
}

ContentManagerType ContentManager_Random_Text::getType() const noexcept {
    return ContentManagerType::RandomText;
}

std::vector<char> ContentManager_Random_Text::ProcessContent(const std::vector<char> & incomingData) noexcept {
    std::vector<char> data;
    data.resize((*distribution)(generator));
    for (int i = 0; i < data.size(); i++) {
        data[i] = chars(generator);
    }
    return std::move(data);
}

bool ContentManager_Random_Text::PrepareContent() noexcept {
    distribution = std::move(std::unique_ptr<std::uniform_int_distribution<int>>(new std::uniform_int_distribution<int>(getMin(), getMax())));
    return true;
}

void ContentManager_Random_Text::CleanupContent() noexcept {
    // do nothing
}
