#include <arpa/inet.h>
#include "commonheaders.h"

CommonHeaders::CommonHeaders() {
}

bool CommonHeaders::read(std::unique_ptr<Protocol> & protocol, std::vector<char> & content) {
    std::vector<char> signature(4 + sizeof(uint32_t));
    if (protocol->read(signature)) {
        if (signature.size() == (4 + sizeof(uint32_t)) &&
            memcmp(&signature[0], "TRAF", 4) == 0) {
                // next 4 bytes is length in network order and includes the header
            uint32_t length = ntohl(*(uint32_t *)(&(signature[4])));
            content.resize(length - (4 + sizeof(uint32_t)));
            if (content.size() == 0) {
                return true;
            }
            return protocol->read(content);
        }
    }
    return false;
}

bool CommonHeaders::write(std::unique_ptr<Protocol> & protocol, const std::vector<char> & content) {
    std::vector<char> signature(4 + sizeof(uint32_t));
    memcpy(&signature[0], "TRAF", 4);
    uint32_t length = htonl(8 + content.size());
    memcpy(&signature[4], &length, sizeof(uint32_t));
    if (protocol->write(signature)) {
        return protocol->write(content);
    }
    return false;
}

unsigned CommonHeaders::getVersion() const {
    return 2;
}
