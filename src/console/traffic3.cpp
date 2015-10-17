#include <iostream>
#include "host.h"
#include "protocol.h"
#include "protocol_tcp4.h"
#include "server.h"
#include "contentmanagerfactory.h"

int main(int argc, char ** argv) {
    // TODO
    ProtocolTCP4 protocol;
    ContentManagerFactory contentManagerFactory(ContentManagerFactory::Type::RandomText);
    Server server(protocol, contentManagerFactory);
    Host port10000("0.0.0.0", 10000);
    if (!server.addPort(port10000)) {
        std::cerr << "Unable to listen on port 10000 TCP" << std::endl;
    }
    getchar();
    return 0;
}
