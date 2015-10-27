#pragma once

#include <vector>
#include "protocol.h"

class CommonHeaders {
public:
    CommonHeaders();
    virtual bool read(std::unique_ptr<Protocol> & protocol, std::vector<char> & content);
    virtual bool write(std::unique_ptr<Protocol> & protocol, const std::vector<char> & content);
    virtual unsigned getVersion() const;
protected:
private:
};