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

#pragma once

#pragma once

#include <thread>
#include <random>
#include <atomic>
#include "contentmanager.h"
#include "protocol.h"
#include "commonheaders.h"

// place holder class, derive new ContentManagers from this class
class ContentManagerBase : public ContentManager {
public:
    ContentManagerBase(std::unique_ptr<Protocol> _protocol, CommonHeaders &_headerHandler, bool isServer);
    ContentManagerBase(ContentManagerBase && other);
    ContentManagerBase & operator=(ContentManagerBase&& other);
    virtual ~ContentManagerBase();
    virtual bool Stop() noexcept override;
    virtual bool Start() noexcept override;
    virtual void setMinimumSize(unsigned long size) noexcept override;
    virtual void setMaximumSize(unsigned long size) noexcept override;
    virtual ContentManagerType getType() const noexcept override = 0;
protected:
    void ServerWorker() noexcept;
    void ClientWorker() noexcept;
    virtual std::vector<char> ProcessContent(const std::vector<char> & incomingData) noexcept = 0;
    virtual bool PrepareContent() noexcept = 0;
    virtual void CleanupContent() noexcept = 0;
    unsigned long getMin() const noexcept {
        return minimum;
    }
    unsigned long getMax() const noexcept {
        return maximum;
    }
    void setMax(unsigned long newMax) noexcept {
        maximum = newMax;
    }
private:
    std::unique_ptr<Protocol> protocol;
    unsigned long minimum, maximum;
    std::atomic<bool> started;
    std::atomic<bool> running;
    std::atomic<bool> doExitBeforeStart;
    CommonHeaders & headerHandler;
    std::thread worker;

    ContentManagerBase(const ContentManagerBase &) = delete;
    ContentManagerBase & operator=(const ContentManagerBase &) = delete;
};
