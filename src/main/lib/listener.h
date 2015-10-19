#pragma once

#include "contentmanagerfactory.h"
#include "protocolfactory.h"

class Listener {
public:
    Listener(const Host & _host, ProtocolFactory & protocolFactory, ContentManagerFactory & contentManagerFactory);
    Listener(Listener && other);
    virtual ~Listener();
    Listener & operator=(Listener&& other);
    bool operator==(const Host & other) const;
    const Host & getHost() const;
    bool inErrorState() const noexcept;
    bool Stop();
protected:
    void listen();
private:
    Host host;
    std::unique_ptr<Protocol> protocol;
    ContentManagerFactory & _contentManagerFactory;
    std::vector<std::unique_ptr<ContentManager>> contentManagers;
    std::thread thread;
    bool errorState;
    
    Listener(const Listener &) = delete;
    Listener & operator=(const Listener &) = delete;
 };


