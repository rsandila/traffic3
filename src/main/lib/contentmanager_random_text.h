#pragma once

#include "contentmanager.h"
#include "protocol.h"

// place holder class
class ContentManager_Random_Text : public ContentManager {
public:
    ContentManager_Random_Text(Protocol & _protocol) : protocol(_protocol), worker(std::thread(std::bind(&ContentManager_Random_Text::Worker, this)))  {
    };
    virtual bool Stop() override {
        if (protocol.getState() != Protocol::ProtocolState::CLOSED) {
            protocol.close();
            return true;
        }
        return false;
    };
protected:
    void Worker() {
        std::vector<char> data;
        data.resize(1024);
        // TODO - do real random text
        while (protocol.read(data)) {
            for (int i = 0; i < data.size(); i++) {
                data[i] = data[i] + 1;
            }
            protocol.write(data);
            data.resize(1024);
        }
    }
private:
    Protocol & protocol;
    std::thread worker;
};