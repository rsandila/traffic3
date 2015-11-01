/*
 * Copyright (C) 2003 Robert Sandilands (Pty) Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 USA.
 */

#include <iostream>
#include "host.h"
#include "protocolfactory.h"
#include "server.h"
#include "contentmanagerfactory.h"
#include "commonheaders.h"
#include "client.h"
#include "logging.h"

INITIALIZE_EASYLOGGINGPP

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
    if (client.startClients(1, 1, protocolFactory, contentManagerFactory, port10000)) {
        getchar();
        return 0;
    } else {
        std::cerr << "Unable to start clients" << std::endl;
    }
    return 1;
}

int main(int argc, char ** argv) {
    // TODO
    START_EASYLOGGINGPP(argc, argv);
    switch (argv[1][0]) {
            case 's':
                return beServer();
            case 'c':
                return beClient();
    }
    return 10;
}
