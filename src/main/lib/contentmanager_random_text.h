#pragma once

#include <thread>
#include <random>
#include "contentmanager.h"
#include "protocol.h"

// place holder class
class ContentManager_Random_Text : public ContentManager {
public:
    ContentManager_Random_Text(std::unique_ptr<Protocol> _protocol) : started(false), protocol(std::move(_protocol)),
            min(0), max(1024000), worker(std::thread(std::bind(&ContentManager_Random_Text::Worker, this)))  {
    };
    ContentManager_Random_Text(ContentManager_Random_Text && other) : protocol(std::move(other.protocol)) {
        min = other.min;
        max = other.max;
        worker = std::move(other.worker);
        started = false;
        if (other.started) {
            started = true;
        }
        other.started = false;
    }
    ContentManager_Random_Text & operator=(ContentManager_Random_Text&& other) {
        protocol = std::move(other.protocol);
        min = other.min;
        max = other.max;
        worker = std::move(other.worker);
        started = false;
        if (other.started) {
            started = true;
        }
        other.started = false;
        return *this;
    }
    virtual ~ContentManager_Random_Text() {
        Stop();
        worker.join();
    }
    virtual bool Stop() override {
        if (protocol->getState() != Protocol::ProtocolState::CLOSED) {
            protocol->close();
            return true;
        }
        return false;
    };
    virtual bool Start() override {
        if (started) {
            return false;
        }
        started = true;
        return true;
    };
    virtual void setMinimumSize(unsigned size) noexcept override {
        min = size;
    };
    virtual void setMaximumSize(unsigned size) noexcept override {
        max = size;
        if (max < min) {
            min = size;
        }
    };
    virtual ContentManagerType getType() const noexcept override {
        return ContentManagerType::RandomText;
    }
protected:
    void Worker() {
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        while (!started) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        std::default_random_engine generator(seed);
        std::uniform_int_distribution<int> distribution(min, max);
        std::uniform_int_distribution<char> chars(32, 127);
        std::vector<char> data;
        std::vector<char> in_data;
        in_data.resize(1024);
        data.resize(distribution(generator));
        
        while (protocol->read(in_data)) {
            // TODO - verify valid header?
            in_data.resize(1024);
            for (int i = 0; i < data.size(); i++) {
                data[i] = chars(generator);
            }
            protocol->write(data);
        }
    }
private:
    std::unique_ptr<Protocol> protocol;
    std::thread worker;
    unsigned min, max;
    std::atomic<bool> started;
    
    ContentManager_Random_Text(const ContentManager_Random_Text &) = delete;
    ContentManager_Random_Text & operator=(const ContentManager_Random_Text &) = delete;
};