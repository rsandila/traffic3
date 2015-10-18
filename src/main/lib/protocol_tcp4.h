#pragma once

#include <mutex>
#include "protocol.h"

class ProtocolTCP4: public Protocol {
public:
    ProtocolTCP4();
    virtual ~ProtocolTCP4();
    virtual bool read(std::vector<char> & data) override;
    virtual bool write(const std::vector<char> & data) override;
    virtual ProtocolState getState() override;
    virtual bool isReady(const ProtocolState & expected, int timeoutInMilliseconds) override;
    virtual bool listen(const Host & host) override;
    virtual bool connect(const Host & host) override;
    virtual void close() override;
    virtual ProtocolType getType() override;
    virtual std::unique_ptr<Protocol> waitForNewConnection() override;
protected:
    Host host;
    ProtocolType type;
    std::mutex lock;
    int socket;
    ProtocolState state;
private:
    ProtocolTCP4(int socket, socklen_t len, const struct sockaddr * addr);
    ProtocolTCP4(const ProtocolTCP4 &) = delete;
    ProtocolTCP4 & operator=(const ProtocolTCP4 &) = delete;
    ProtocolTCP4(ProtocolTCP4 && other);
};