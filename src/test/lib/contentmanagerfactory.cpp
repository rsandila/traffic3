// Testing contentmanagerfactory.h
#include <memory>
#include <thread>
#include "catch.hpp"
#include "contentmanagerfactory.h"

TEST_CASE("Testing contentmanagerfactory.h", "[content]") {
    SECTION("Test random text") {
        ContentManagerFactory test(ContentManagerType::RandomText);
        Protocol dummy;
        
        std::unique_ptr<ContentManager> text(test.createContentManager(dummy));
        // ContentManager * text = test.createContentManager(dummy);
        REQUIRE(text->getType() == ContentManagerType::RandomText);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        // delete text;
    }
}