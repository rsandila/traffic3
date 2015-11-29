/*
 * Copyright (C) 2015 Robert Sandilands
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
#include <fstream>
#include <iostream>
#include "lib/host.h"
#include "protocol/protocolfactory.h"
#include "lib/server.h"
#include "contentmanager/contentmanagerfactory.h"
#include "lib/commonheaders.h"
#include "lib/client.h"
#include "lib/logging.h"
#include "cmdline.h"
#include "rest/restheaders.h"

INITIALIZE_EASYLOGGINGPP

enum class ModeType {
    InvalidMode,
    ServerMode,
    ClientMode,
    RestMode
};

static std::map<std::string, ModeType> modeMap {
    {"server", ModeType::ServerMode},
    {"client", ModeType::ClientMode},
    {"rest", ModeType::RestMode} };
static std::map<std::string, Host::ProtocolPreference> protocolPreferenceMap {
    {"tcp4", Host::ProtocolPreference::IPV4},
    {"tcp6", Host::ProtocolPreference::IPV6},
    {"udp4", Host::ProtocolPreference::IPV4},
    {"udp6", Host::ProtocolPreference::IPV6} };
static std::map<std::string, ProtocolType> protocolMap {
    {"tcp4", ProtocolType::TCP4},
    {"tcp6", ProtocolType::TCP6},
    {"udp4", ProtocolType::UDP4},
    {"udp6", ProtocolType::UDP6} };
static std::map<std::string, ContentManagerType> contentManagerMap {
    {"randomtext", ContentManagerType::RandomText},
    {"randombinary", ContentManagerType::RandomBinary},
    {"fixed", ContentManagerType::Fixed},
    {"echo", ContentManagerType::Echo}
};

int beServer(const cmdline::parser & options) {
    CommonHeaders headers;
    ProtocolFactory protocolFactory(protocolMap[options.get<std::string>("protocol")]);
    ContentManagerFactory contentManagerFactory(contentManagerMap[options.get<std::string>("type")], options.get<unsigned>("min"), options.get<unsigned>("max"), headers);
    Server server(protocolFactory, contentManagerFactory);
    Host port10000(options.get<std::string>("interface"), options.get<unsigned>("port"),
                   protocolPreferenceMap[options.get<std::string>("protocol")]);
    if (!server.addPort(port10000)) {
        std::cerr << "Unable to listen on port " << options.get<unsigned>("port") << " " << options.get<std::string>("protocol") << " on interface " << options.get<std::string>("interface") << std::endl;
        return 1;
    }
    getchar();
    return 0;
}

int beClient(const cmdline::parser & options) {
    ProtocolFactory protocolFactory(protocolMap[options.get<std::string>("protocol")]);
    CommonHeaders headers;
    ContentManagerFactory contentManagerFactory(contentManagerMap[options.get<std::string>("type")], options.get<unsigned>("min"), options.get<unsigned>("max"), headers);
    Host port10000(options.get<std::string>("host"), options.get<unsigned>("port"),
                   protocolPreferenceMap[options.get<std::string>("protocol")]);
    Client client;
    if (client.startClients(1, options.get<unsigned>("count"), protocolFactory, contentManagerFactory, port10000)) {
        getchar();
        return 0;
    } else {
        std::cerr << "Unable to start clients on " << options.get<unsigned>("port") << " " << options.get<std::string>("protocol") << " on interface " << options.get<std::string>("interface") << " to server " << options.get<std::string>("host") << std::endl;
    }
    return 1;
}

int beRest(const cmdline::parser & options) {
    ProtocolFactory protocolFactory(protocolMap[options.get<std::string>("protocol")]);
    RestHeaders headers;
    ContentManagerFactory contentManagerFactory(contentManagerMap[options.get<std::string>("type")], options.get<unsigned>("min"), options.get<unsigned>("max"), headers);
    Server server(protocolFactory, contentManagerFactory);
    Host port10000(options.get<std::string>("interface"), options.get<unsigned>("port"),
                   protocolPreferenceMap[options.get<std::string>("protocol")]);
    if (!server.addPort(port10000)) {
        std::cerr << "Unable to listen on port " << options.get<unsigned>("port") << " " << options.get<std::string>("protocol") << " on interface " << options.get<std::string>("interface") << std::endl;
        return 1;
    }
    getchar();
    return 0;
}

void print_usage(const std::string argv0) {
    std::cerr << std::endl << "\tUsage: " << argv0 << " server|client ...." << std::endl;
}

static const char * TRAFFIC_CONF = "traffic3.logging.conf";

int main(int argc, char ** argv) {
    // TODO
	{
		std::ifstream test(TRAFFIC_CONF);
		if (test.is_open()) {
			el::Configurations conf(TRAFFIC_CONF);
			el::Loggers::reconfigureAllLoggers(conf);
		}
	}
    cmdline::parser options;
    options.add<std::string>("mode", 'o', "Mode [server|client|rest]", true);
    options.add<std::string>("protocol", 'r', "Protocol [tcp4|udp4|tcp6|udp6]", false, "tcp4");
    options.add<std::string>("type", 't', "ContentManager type [randomtext|randombinary|fixed|echo]", false, "randomtext");
    options.add<unsigned>("port", 'p', "Port to connect to or listen on", true);
    options.add<unsigned>("min", 'm', "Minimum value to configure contentmanager with", false, 100);
    options.add<unsigned>("max", 'a', "Maximum value to configure contentmanager with", false, 100000);
    options.add<std::string>("host", 'h', "Host to connect to", false, "127.0.0.1");
    options.add<unsigned>("count", 'c', "Number of clients to use", false, 1);
    options.add<std::string>("interface", 'i', "Interface IP to listen on", false, "0.0.0.0");
    options.parse_check(argc, argv);

    START_EASYLOGGINGPP(argc, argv);
    
    switch (modeMap[options.get<std::string>("mode")]) {
        case ModeType::ServerMode:
            return beServer(options);
        case ModeType::ClientMode:
            return beClient(options);
        case ModeType::RestMode:
            return beRest(options);
        default:
            std::cerr << "Unknown mode: " << options.get<std::string>("mode");
            print_usage(argv[0]);
            break;
    }
    return 10;
}
