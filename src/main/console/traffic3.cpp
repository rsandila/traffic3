#include <iostream>
#include "host.h"
#include "protocolfactory.h"
#include "server.h"
#include "contentmanagerfactory.h"
#include "commonheaders.h"
#include "client.h"


int beServer() {
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

int beClient() {
    ProtocolFactory protocolFactory(ProtocolType::TCP4);
    ContentManagerFactory contentManagerFactory(ContentManagerType::RandomText, 100, 100000, CommonHeaders());
    Host port10000("127.0.0.1", 10000);
    Client client;
    if (client.startClients(1, 10, protocolFactory, contentManagerFactory, port10000)) {
        getchar();
        return 0;
    }
    return 1;
}

int main(int argc, char ** argv) {
    // TODO
    switch (argv[1][0]) {
            case 's':
                return beServer();
            case 'c':
                return beClient();
    }
    return 10;
}
