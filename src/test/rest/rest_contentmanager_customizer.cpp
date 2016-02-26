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
#include "hippomocks.h"
#include "rest/rest_contentmanager_customizer.h"
#include "rest/contentmanager_headers.h"

/*
 This will never work: https://github.com/dascandy/hippomocks/issues/10
 
TEST_CASE("RestContentManagerCustomizer", "[rest][content]") {
    SECTION("nullptr") {
        MockRepository mocks;
        
        std::vector<std::shared_ptr<RestRequestHandler>> restRequestHandlers { std::shared_ptr<RestRequestHandler>(nullptr) };
        std::vector<std::shared_ptr<ErrorPageHandler>> errorPageHandlers { std::shared_ptr<ErrorPageHandler>(nullptr) };
        RestContentManagerCustomizer customizer(restRequestHandlers, errorPageHandlers);
        
        ContentManager_Headers * contentManagerTmp = mocks.Mock<ContentManager_Headers>();
        mocks.NeverCall(contentManagerTmp, ContentManager_Headers::addErrorPageHandlers);
        mocks.NeverCall(contentManagerTmp, ContentManager_Headers::addRestRequestHandlers);
        
        std::unique_ptr<ContentManager> contentManager(contentManagerTmp);
        
        contentManager = std::move(customizer.customize(std::move(contentManager)));
        
        mocks.ExpectCallDestructor(contentManager.get());
        contentManager.reset();
    }
}
 */
// TODO - mocking is not going to work, but the underlying code is way to simple to test any other way...