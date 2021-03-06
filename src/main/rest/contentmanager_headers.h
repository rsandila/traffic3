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

#include <thread>
#include <vector>
#include <random>
#include "contentmanager/contentmanager_base.h"
#include "protocol/protocol.h"
#include "errortypes.h"
#include "errorpage_handler.h"
#include "rest_request_handler.h"

class ContentManager_Headers : public ContentManagerBase {
public:
    ContentManager_Headers(std::unique_ptr<Protocol> _protocol, std::shared_ptr<CommonHeaders> &_headerHandler, bool isServer);
    virtual ~ContentManager_Headers();
    virtual ContentManagerType getType() const noexcept override;
    
    virtual bool addErrorPageHandlers(std::vector<std::shared_ptr<ErrorPageHandler>> & errorHandler);
    virtual bool addRestRequestHandlers(std::vector<std::shared_ptr<RestRequestHandler>> & restHandler);
protected:
    virtual std::vector<char> ProcessContent(const std::vector<char> & incomingData, const Host & host) noexcept override;
    virtual bool PrepareContent() noexcept override;
    virtual void CleanupContent() noexcept override;
    virtual std::vector<char> returnErrorPage(ErrorTypes type, const std::vector<char> & incomingData);

    std::vector<std::shared_ptr<ErrorPageHandler>> errorHandlers;
    std::vector<std::shared_ptr<RestRequestHandler>> restHandlers;
private:
    std::string getLineFromVector(const std::vector<char> & data, unsigned int & startOffset) noexcept;
    void trimString(std::string & line) const noexcept;
    
};
