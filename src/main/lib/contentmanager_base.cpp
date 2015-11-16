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
#include <WinSock2.h>
#endif
#include <thread>
#include "logging.h"
#include "contentmanager_base.h"

ContentManagerBase::ContentManagerBase(std::unique_ptr<Protocol> _protocol, CommonHeaders &_headerHandler, bool isServer) :
        protocol(std::move(_protocol)), minimum(0), maximum(1024000), started(false), running(false), doExitBeforeStart(false), headerHandler(_headerHandler),
        worker(std::thread(std::bind((isServer)?&ContentManagerBase::ServerWorker:&ContentManagerBase::ClientWorker, this)))  {
}

ContentManagerBase::ContentManagerBase(ContentManagerBase && other) :
        protocol(std::move(other.protocol)), headerHandler(other.headerHandler) {
    minimum = other.minimum;
    maximum = other.maximum;
    worker = std::move(other.worker);
    started = false;
    if (other.started) {
        started = true;
    }
    other.started = false;
    running = false;
    if (other.running) {
        running = true;
    }
    other.running = false;
    doExitBeforeStart = false;
    if (other.doExitBeforeStart) {
        doExitBeforeStart = true;
    }
}

ContentManagerBase & ContentManagerBase::operator=(ContentManagerBase&& other) {
    protocol = std::move(other.protocol);
    minimum = other.minimum;
    maximum = other.maximum;
    worker = std::move(other.worker);
    headerHandler = other.headerHandler;
    started = false;
    if (other.started) {
        started = true;
    }
    other.started = false;
    if (other.running) {
        running = true;
    }
    other.running = false;
    doExitBeforeStart = false;
    if (other.doExitBeforeStart) {
        doExitBeforeStart = true;
    }
    return *this;
}

ContentManagerBase::~ContentManagerBase() {
    Stop();
}

bool ContentManagerBase::Stop() noexcept {
    if (protocol->getState() != Protocol::ProtocolState::CLOSED) {
        if (!started) {
            doExitBeforeStart = true;
        }
        while (!running && started) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        protocol->close();
    }
    if (worker.joinable()) {
        worker.join();
    }
    return true;
}

bool ContentManagerBase::Start() noexcept {
    if (started) {
        return false;
    }
    started = true;
    return true;
}

void ContentManagerBase::setMinimumSize(unsigned long size) noexcept {
    minimum = size;
}

void ContentManagerBase::setMaximumSize(unsigned long size) noexcept {
    setMax(size);
    if (getMax() < minimum) {
        minimum = size;
    }
}

void ContentManagerBase::ClientWorker() noexcept {
    while (!started) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if (doExitBeforeStart) {
            return;
        }
    }
    
    if (!PrepareContent()) {
        return;
    }
    std::vector<char> inData;
    
    Host hostState = Host::ALL_INTERFACES6;
    running = true;
    do {
        std::vector<char> outData = ProcessContent(inData);
        if (!headerHandler.write(protocol, outData, hostState)) {
            LOG(WARNING) << std::this_thread::get_id() << " write failed " << errno << std::endl;
            break;
        }
    } while (headerHandler.read(protocol, inData, hostState));
    LOG(WARNING) << std::this_thread::get_id() << " read failed " << errno << std::endl;
    CleanupContent();
}

void ContentManagerBase::ServerWorker() noexcept {
    while (!started) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if (doExitBeforeStart) {
            return;
        }
    }
    
    if (!PrepareContent()) {
        return;
    }
    std::vector<char> inData;
    
    Host hostState = Host::ALL_INTERFACES6;
    running = true;
    while (headerHandler.read(protocol, inData, hostState)) {
        std::vector<char> outData = ProcessContent(inData);
        if (!headerHandler.write(protocol, outData, hostState)) {
            LOG(WARNING) << std::this_thread::get_id() << " write failed " << errno << std::endl;
            break;
        }
    }
    LOG(WARNING) << std::this_thread::get_id() << " read failed " << errno << std::endl;
    CleanupContent();
}
