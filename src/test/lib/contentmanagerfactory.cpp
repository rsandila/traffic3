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
// Testing contentmanagerfactory.h
#include <memory>
#include <thread>
#include "catch.hpp"
#include "contentmanager/contentmanagerfactory.h"
#include "contentmanager/contentmanager_customizer.h"

TEST_CASE("Testing contentmanagerfactory.h", "[content]") {
    SECTION("Test random text") {
        CommonHeaders commonHeaders;
        std::unique_ptr<ContentManagerCustomizer> contentManagerCustomizer(new ContentManagerCustomizer(100, 100000));
        std::shared_ptr<ContentManagerFactory> test(new ContentManagerFactory(ContentManagerType::RandomText,
                                                                              commonHeaders, contentManagerCustomizer));
        std::unique_ptr<Protocol> dummy(new Protocol());
        
        std::unique_ptr<ContentManager> text(test->createContentManager(std::move(dummy), true));
        text->Start();
        REQUIRE(text->getType() == ContentManagerType::RandomText);
        text->Stop();
    }
    SECTION("Test fixed") {
        CommonHeaders commonHeaders;
        std::unique_ptr<ContentManagerCustomizer> contentManagerCustomizer(new ContentManagerCustomizer(100, 100000));
        ContentManagerFactory test(ContentManagerType::Fixed, commonHeaders, contentManagerCustomizer);
        std::unique_ptr<Protocol> dummy(new Protocol());
        
        std::unique_ptr<ContentManager> text(test.createContentManager(std::move(dummy), true));
        text->Start();
        REQUIRE(text->getType() == ContentManagerType::Fixed);
        text->Stop();
    }
    SECTION("Test echo") {
        CommonHeaders commonHeaders;
        std::unique_ptr<ContentManagerCustomizer> contentManagerCustomizer(new ContentManagerCustomizer(100, 100000));
        ContentManagerFactory test(ContentManagerType::Echo, commonHeaders, contentManagerCustomizer);
        std::unique_ptr<Protocol> dummy(new Protocol());
        
        std::unique_ptr<ContentManager> text(test.createContentManager(std::move(dummy), true));
        text->Start();
        REQUIRE(text->getType() == ContentManagerType::Echo);
        text->Stop();
    }

}