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
#include <thread>
#include <random>
#include "contentmanager_random_text.h"
#include "logging.h"

ContentManager_Random_Text::ContentManager_Random_Text(std::unique_ptr<Protocol> _protocol, CommonHeaders &_headerHandler, bool isServer) :
        ContentManagerBase(std::move(_protocol), _headerHandler, isServer),
        generator(std::chrono::system_clock::now().time_since_epoch().count()),
        chars(32, 127), distribution(nullptr) {
}

ContentManager_Random_Text::~ContentManager_Random_Text() {
}

ContentManagerType ContentManager_Random_Text::getType() const noexcept {
    return ContentManagerType::RandomText;
}

std::vector<char> ContentManager_Random_Text::ProcessContent(const std::vector<char> & incomingData) noexcept {
    LOG(DEBUG) << "entering with with " << incomingData.size() << std::endl;
    std::vector<char> data;
    data.resize((*distribution)(generator));
    for (unsigned long i = 0; i < data.size(); i++) {
        data[i] = (char)chars(generator);
    }
    LOG(DEBUG) << "exiting with " << data.size() << std::endl;
    return std::move(data);
}

bool ContentManager_Random_Text::PrepareContent() noexcept {
    distribution = std::move(std::unique_ptr<std::uniform_int_distribution<int>>(new std::uniform_int_distribution<int>(getMin(), getMax())));
    return true;
}

void ContentManager_Random_Text::CleanupContent() noexcept {
    // do nothing
}
