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

#include "rest_contentmanager_customizer.h"
#include "contentmanager_headers.h"

RestContentManagerCustomizer::RestContentManagerCustomizer(
                            std::vector<std::shared_ptr<RestRequestHandler>> & restRequestHandlers,
                            std::vector<std::shared_ptr<ErrorPageHandler>> & errorPageHandlers)
        : ContentManagerCustomizer(0, 0), restRequestHandlerList(restRequestHandlers),
            errorPageHandlerList(errorPageHandlers) {
}

std::unique_ptr<ContentManager> RestContentManagerCustomizer::customize(std::unique_ptr<ContentManager> contentManager) {
    ContentManager_Headers * contentManagerHeaders = dynamic_cast<ContentManager_Headers *>(contentManager.get());
    if (contentManagerHeaders != nullptr) {
        contentManagerHeaders->addErrorPageHandlers(errorPageHandlerList);
        contentManagerHeaders->addRestRequestHandlers(restRequestHandlerList);
    }
    return contentManager;
}
