#include <thread>
#include <random>
#include "contentmanager_random_text.h"

// TODO - refactor to simplify derived classes significantly... this is way too hairy

ContentManager_Random_Text::ContentManager_Random_Text(std::unique_ptr<Protocol> _protocol) : started(false),
        running(false), protocol(std::move(_protocol)), min(0), max(1024000),
        worker(std::thread(std::bind(&ContentManager_Random_Text::Worker, this)))  {
}

ContentManager_Random_Text::ContentManager_Random_Text(ContentManager_Random_Text && other) :
            protocol(std::move(other.protocol)) {
    min = other.min;
    max = other.max;
    worker = std::move(other.worker);
    started = false;
    if (other.started) {
       started = true;
    }
    other.started = false;
    running = false;
    if (other.running) {
       running = true;
    }
    other.running = false;
}

ContentManager_Random_Text & ContentManager_Random_Text::operator=(ContentManager_Random_Text&& other) {
    protocol = std::move(other.protocol);
    min = other.min;
    max = other.max;
    worker = std::move(other.worker);
    started = false;
    if (other.started) {
       started = true;
    }
    other.started = false;
    if (other.running) {
       running = true;
    }
    other.running = false;
    return *this;
}

ContentManager_Random_Text::~ContentManager_Random_Text() {
    Stop();
}

bool ContentManager_Random_Text::Stop() {
    if (protocol->getState() != Protocol::ProtocolState::CLOSED) {
        if (!started) {
            doExitBeforeStart = true;
        }
        while (!running && started) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        protocol->close();
    }
    worker.join();
    return true;
}

bool ContentManager_Random_Text::Start() {
    if (started) {
        return false;
    }
    started = true;
    return true;
}

void ContentManager_Random_Text::setMinimumSize(unsigned size) noexcept {
    min = size;
}

void ContentManager_Random_Text::setMaximumSize(unsigned size) noexcept {
    max = size;
    if (max < min) {
       min = size;
    }
}

ContentManagerType ContentManager_Random_Text::getType() const noexcept {
    return ContentManagerType::RandomText;
}

void ContentManager_Random_Text::Worker() {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    while (!started) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if (doExitBeforeStart) {
            return;
        }
    }
    std::default_random_engine generator(seed);
    std::uniform_int_distribution<int> distribution(min, max);
    std::uniform_int_distribution<char> chars(32, 127);
    std::vector<char> data;
    std::vector<char> in_data;
    in_data.resize(1024);
    data.resize(distribution(generator));
        
    running = true;
    while (protocol->read(in_data)) {
        // TODO - verify valid header?
        in_data.resize(1024);
        for (int i = 0; i < data.size(); i++) {
            data[i] = chars(generator);
        }
        protocol->write(data);
    }
}