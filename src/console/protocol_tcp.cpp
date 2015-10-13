#include <sys/socket.h>
#include <poll.h>
#include <netinet/in.h>
#include "protocol_tcp.h"

ProtocolTCP4::ProtocolTCP4() : host(Host::ALL_INTERFACES), active(false), port(0), type(ProtocolType::NONE),
        socket(-1), state(ProtocolState::CLOSED) {
}

ProtocolTCP4::ProtocolTCP4(int newSocket, socklen_t len, struct sockaddr_in addr) : host(len, addr), active(true), port(0),
        type(ProtocolType::CLIENT), socket(newSocket), state(ProtocolState::OPEN) {
}

ProtocolTCP4::ProtocolTCP4(ProtocolTCP4 && other) : host(other.host), active(other.active), port(other.port),
        type(other.type), socket(other.socket), state(other.state) {
    other.socket = -1;
    other.host = Host::ALL_INTERFACES;
    other.active = false;
    other.port = 0;
    other.type = ProtocolType::NONE;
    other.state = ProtocolState::CLOSED;
}

ProtocolTCP4::~ProtocolTCP() {
    std::unique_lock<std::mutex> lck(lock);
    if (state != ProtocolState::CLOSED) {
        shutdown(socket, SHUT_RDWR);
        close(socket);
        socket = 0;
    }
}

bool ProtocolTCP4::read(std::vector<char> & data) {
    std::unique_lock<std::mutex> lck(lock);
    if (state == ProtocolState::CLOSED) {
        return false;
    }
    ssize_t numRead = ::read(socket, &data[0], data.size());
    if (numRead > 0) {
        data.resize(numRead);
    }
    return numRead > 0;
}

bool ProtocolTCP4::write(std::vector<char> & data) {
    std::unique_lock<std::mutex> lck(lock);
    if (state == ProtocolState::CLOSED) {
        return false;
    }
    ssize_t numWritten = ::write(socket, &data[0], data.size());
    return numWritten == data.size();;
}

ProtocolTCP4::ProtocolState ProtocolTCP4::getState() {
    std::unique_lock<std::mutex> lck(lock);
    return state;
}

bool ProtocolTCP4::isReady(const ProtocolState & expected, int timeoutInMilliseconds) {
    std::unique_lock<std::mutex> lck(lock);
    pollfd fd;
    fd.fd = socket;
    fd.revents = 0;
    switch (expected) {
        case Protocol::ProtocolState::CLOSED:
            return state == ProtocolState::CLOSED;
        case Protocol::ProtocolState::OPEN:
            return state == ProtocolState::OPEN;
        case Protocol::ProtocolState::READ_READY:
            fd.events = POLLRDNORM;
            if (poll(&fd, 1, timeoutInMilliseconds) > 0) {
                if (fd.revents & POLLRDNORM) {
                    return true;
                }
            }
            return false;
        case Protocol::ProtocolState::WRITE_READY:
            fd.events = POLLWRNORM;
            if (poll(&fd, 1, timeoutInMilliseconds) > 0) {
                if (fd.revents & POLLWRNORM) {
                    return true;
                }
            }
            return false;
        default:
            return false;
    }
}

bool ProtocolTCP4::listen(const Host & host, int port) {
    std::unique_lock<std::mutex> lck(lock);
    if (state != ProtocolState::CLOSED || type != ProtocolType::NONE) {
        return false;
    }
    this->host = host;
    socket = ::socket(PF_INET, SOCK_STREAM, 6);
    if (::bind(socket, host.getSockAddress(), host.getSockAddressLen()) == 0) {
        type = ProtocolType::SERVER;
        state = ProtocolState::OPEN;
        return true;
    }
    return false;
}

bool ProtocolTCP4::connect(const Host & host, int port) {
    std::unique_lock<std::mutex> lck(lock);
    if (state != ProtocolState::CLOSED || type != ProtocolType::NONE) {
        return false;
    }
    this->host = host;
    socket = ::socket(PF_INET, SOCK_STREAM, 6);
    if (::connect(socket, host.getSockAddress(), host.getSockAddressLen()) == 0) {
        type = ProtocolType::CLIENT;
        state = ProtocolState::OPEN;
        return true;
    }
    return false;
}

ProtocolTCP4::ProtocolType ProtocolTCP4::getType() {
    std::unique_lock<std::mutex> lck(lock);
    return type;
}

Protocol ProtocolTCP4::waitForNewConnection() {
    struct sockaddr_in addr;
    socklen_t addrlen=sizeof(addr);
    int newSocket=::accept(socket, (sockaddr *)&addr, &addrlen );
    return std::move(ProtocolTCP4(newSocket, addrlen, addr));
}
