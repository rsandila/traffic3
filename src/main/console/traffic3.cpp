#include <iostream>
#include "host.h"
#include "protocolfactory.h"
#include "server.h"
#include "contentmanagerfactory.h"
#include "commonheaders.h"

int main(int argc, char ** argv) {
    // TODO
    ProtocolFactory protocolFactory(ProtocolType::TCP4);
    ContentManagerFactory contentManagerFactory(ContentManagerType::RandomText, 100, 100000, CommonHeaders());
    Server server(protocolFactory, contentManagerFactory);
    Host port10000("0.0.0.0", 10000);
    if (!server.addPort(port10000)) {
        std::cerr << "Unable to listen on port 10000 TCP" << std::endl;
    }
    getchar();
    return 0;
}
