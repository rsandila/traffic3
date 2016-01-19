/*
 * Copyright (C) 2016 Robert Sandilands
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

#include "catch.hpp"
#include "contentmanager/contentmanager_customizer.h"
#include "contentmanager/contentmanager.h"
#include "lib/common.h"

TEST_CASE("Content Manager Customizer", "[content][customizer]") {
    
    class MockContentManager: public ContentManager {
    public:
        MockContentManager() : maximum(0), minimum(0) {
        }
        virtual void setMinimumSize(unsigned long size) noexcept override {
            minimum = size;
        };
        virtual void setMaximumSize(unsigned long size) noexcept override {
            maximum = size;
        };
        unsigned long maximum;
        unsigned long minimum;
    };
    SECTION("Good") {
        ContentManagerCustomizer custom1(10, 100);
        std::unique_ptr<ContentManager> mockContentManager(new MockContentManager());
        std::unique_ptr<ContentManager> returnValue = custom1.customize(std::move(mockContentManager));
        
        REQUIRE(mockContentManager.get() == nullptr);
        REQUIRE(returnValue.get() != nullptr);
        MockContentManager * returnValueCast = dynamic_cast<MockContentManager *>(returnValue.get());
        REQUIRE(returnValueCast != nullptr);
        REQUIRE(returnValueCast->minimum == 10);
        REQUIRE(returnValueCast->maximum == 100);
        
        nlohmann::json json = custom1.toJson();
        REQUIRE(json.size() == 3);
        REQUIRE(10 == json["min"].get<unsigned>());
        REQUIRE(100 == json["max"].get<unsigned>());
        REQUIRE("ContentManagerCustomizer" == json["type"].get<std::string>());
    }
}
