#pragma once

class ContentManager {
public:
    ContentHandler contentHandlerFactory(const Host & local, int local_port, const Host & remote, int remote_port, Protocol & protocol) = 0;
};