#pragma once

#include <vector>
#include "host.h"

class Protocol {
public:
    enum class ProtocolState {
        READ_READY,
        WRITE_READY,
        OPEN,
        CLOSED
    };
    enum class ProtocolType {
        SERVER,
        CLIENT,
        NONE
    };
    virtual bool read(std::vector<char> & data) { return false; };
    virtual bool write(std::vector<char> & data) { return false; };
    virtual ProtocolState getState() { return ProtocolState::CLOSED; };
    virtual bool isReady(const ProtocolState & expected, int timeoutInMilliseconds) { return false; };
    virtual bool listen(const Host & host, int port) { return false; };
    virtual bool connect(const Host & host, int port) { return false; };
    virtual bool isServer() { return getType() == ProtocolType::SERVER; };
    virtual bool isClient() { return getType() == ProtocolType::CLIENT; };
    virtual ProtocolType getType() { return ProtocolType::NONE; };
    virtual Protocol waitForNewConnection() { return std::move(Protocol()); };
protected:
private:
};