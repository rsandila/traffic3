#include <thread>
#include <random>
#include "contentmanager_base.h"

ContentManagerBase::ContentManagerBase(std::unique_ptr<Protocol> _protocol, CommonHeaders &_headerHandler, bool isServer) : started(false),
        running(false), protocol(std::move(_protocol)), min(0), max(1024000), headerHandler(_headerHandler),
        worker(std::thread(std::bind((isServer)?&ContentManagerBase::ServerWorker:&ContentManagerBase::ClientWorker, this)))  {
}

ContentManagerBase::ContentManagerBase(ContentManagerBase && other) :
        protocol(std::move(other.protocol)), headerHandler(other.headerHandler) {
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

ContentManagerBase & ContentManagerBase::operator=(ContentManagerBase&& other) {
    protocol = std::move(other.protocol);
    min = other.min;
    max = other.max;
    worker = std::move(other.worker);
    headerHandler = other.headerHandler;
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

ContentManagerBase::~ContentManagerBase() {
    Stop();
}

bool ContentManagerBase::Stop() noexcept {
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

bool ContentManagerBase::Start() noexcept {
    if (started) {
        return false;
    }
    started = true;
    return true;
}

void ContentManagerBase::setMinimumSize(unsigned size) noexcept {
    min = size;
}

void ContentManagerBase::setMaximumSize(unsigned size) noexcept {
    max = size;
    if (max < min) {
        min = size;
    }
}

void ContentManagerBase::ClientWorker() noexcept {
    while (!started) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if (doExitBeforeStart) {
            return;
        }
    }
    
    if (!PrepareContent()) {
        return;
    }
    std::vector<char> inData;
    
    running = true;
    do {
        std::vector<char> outData = ProcessContent(inData);
        if (!headerHandler.write(protocol, outData)) {
            break;
        }
    } while (headerHandler.read(protocol, inData));
    CleanupContent();
}

void ContentManagerBase::ServerWorker() noexcept {
    while (!started) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if (doExitBeforeStart) {
            return;
        }
    }
    
    if (!PrepareContent()) {
        return;
    }
    std::vector<char> inData;
    
    running = true;
    while (headerHandler.read(protocol, inData)) {
        std::vector<char> outData = ProcessContent(inData);
        if (!headerHandler.write(protocol, outData)) {
            break;
        }
    }
    CleanupContent();
}
