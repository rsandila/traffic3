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

#include <vector>
#include "protocol/protocol.h"
#include "lib/commonheaders.h"

class RestHeaders : public CommonHeaders {
public:
    RestHeaders();
    virtual ~RestHeaders();
    virtual bool read(std::unique_ptr<Protocol> & protocol, std::vector<char> & content, Host & hostState) override;
    virtual bool write(std::unique_ptr<Protocol> & protocol, const std::vector<char> & content, const Host & hostState) override;
    virtual unsigned getVersion() const override;
protected:
    uint32_t findStringInVector(const std::vector<char> & buffer, uint32_t offset, uint32_t max_offset, const std::string & toFind) const noexcept;
private:
};
