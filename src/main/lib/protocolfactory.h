#pragma once

#include "protocol.h"
#include "protocol_tcp4.h"
#include "protocoltype.h"

class ProtocolFactory {
public:
    ProtocolFactory(const ProtocolType & _type) : type(_type) {
    };
    virtual std::unique_ptr<Protocol> createProtocol() {
        switch (type) {
            case ProtocolType::TCP4:
                return std::unique_ptr<Protocol>(new ProtocolTCP4());
            default:
                return std::unique_ptr<Protocol>(new Protocol());
        }
    };
private:
    ProtocolType type;
};