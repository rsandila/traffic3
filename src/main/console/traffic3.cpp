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

#ifdef _MSC_VER
// TODO - find new command-line parser
int main(int argc, char **argv) {
	return -1;
}
#else
#include <iostream>
#include "host.h"
#include "protocolfactory.h"
#include "server.h"
#include "contentmanagerfactory.h"
#include "commonheaders.h"
#include "client.h"
#include "logging.h"
#include "cmdline.h"

INITIALIZE_EASYLOGGINGPP

ContentManagerType mapStringToContentManagerType(const std::string & value) {
    switch (value[0]) {
            case 'r':
                if (value == "randomtext") {
                    return ContentManagerType::RandomText;
                }
                if (value == "randombinary") {
                    return ContentManagerType::RandomBinary;
                }
                break;
            case 'f':
                if (value == "fixed") {
                    return ContentManagerType::Fixed;
                }
            case 'e':
                if (value == "echo") {
                    return ContentManagerType::Echo;
                }
                break;
    }
    return ContentManagerType::None;
}

ProtocolType mapStringToProtocolType(const std::string & value) {
    switch (value[0]) {
        case 't':
            if (value == "tcp4") {
                return ProtocolType::TCP4;
            }
            if (value == "tcp6") {
                return ProtocolType::TCP6;
            }
            break;
        case 'u':
            if (value == "udp4") {
                return ProtocolType::UDP4;
            }
            if (value == "udp6") {
                return ProtocolType::UDP6;
            }
            break;
        default:
            break;
    }
    return ProtocolType::None;
}

Host::ProtocolPreference mapStringToProtocolPreference(const std::string & value) {
    switch (value[0]) {
        case 't':
            if (value == "tcp4") {
                return Host::ProtocolPreference::IPV4;
            }
            if (value == "tcp6") {
                return Host::ProtocolPreference::IPV6;
            }
            break;
        case 'u':
            if (value == "udp4") {
                return Host::ProtocolPreference::IPV4;
            }
            if (value == "udp6") {
                return Host::ProtocolPreference::IPV6;
            }
            break;
        default:
            break;
    }
    return Host::ProtocolPreference::ANY;
    
}

int beServer(const cmdline::parser & options) {
    ProtocolFactory protocolFactory(mapStringToProtocolType(options.get<std::string>("protocol")));
    ContentManagerFactory contentManagerFactory(mapStringToContentManagerType(options.get<std::string>("type")), options.get<unsigned>("min"), options.get<unsigned>("max"), CommonHeaders());
    Server server(protocolFactory, contentManagerFactory);
    Host port10000(options.get<std::string>("interface"), options.get<unsigned>("port"), mapStringToProtocolPreference(options.get<std::string>("protocol")));
    if (!server.addPort(port10000)) {
        std::cerr << "Unable to listen on port " << options.get<unsigned>("port") << " " << options.get<std::string>("protocol") << " on interface " << options.get<std::string>("interface") << std::endl;
        return 1;
    }
    getchar();
    return 0;
}

int beClient(const cmdline::parser & options) {
    ProtocolFactory protocolFactory(mapStringToProtocolType(options.get<std::string>("protocol")));
    ContentManagerFactory contentManagerFactory(mapStringToContentManagerType(options.get<std::string>("type")), options.get<unsigned>("min"), options.get<unsigned>("max"), CommonHeaders());
    Host port10000(options.get<std::string>("host"), options.get<unsigned>("port"), mapStringToProtocolPreference(options.get<std::string>("protocol")));
    Client client;
    if (client.startClients(1, options.get<unsigned>("count"), protocolFactory, contentManagerFactory, port10000)) {
        getchar();
        return 0;
    } else {
        std::cerr << "Unable to start clients on " << options.get<unsigned>("port") << " " << options.get<std::string>("protocol") << " on interface " << options.get<std::string>("interface") << " to server " << options.get<std::string>("host") << std::endl;
    }
    return 1;
}

void print_usage(const std::string argv0) {
    std::cerr << std::endl << "\tUsage: " << argv0 << " server|client ...." << std::endl;
}

int main(int argc, char ** argv) {
    // TODO
    enum class ModeType {
        InvalidMode,
        ServerMode,
        ClientMode
    };
    std::map<std::string, ModeType> modeMap { {"server", ModeType::ServerMode}, {"client", ModeType::ClientMode} };
    cmdline::parser options;
    options.add<std::string>("mode", 'o', "Mode [server|client]", true);
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
        default:
            std::cerr << "Unknown mode: " << options.get<std::string>("mode");
            print_usage(argv[0]);
            break;
    }
    return 10;
}
#endif
