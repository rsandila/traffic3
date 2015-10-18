// Testing contentmanagerfactory.h
#include <memory>
#include <thread>
#include "catch.hpp"
#include "contentmanagerfactory.h"

TEST_CASE("Testing contentmanagerfactory.h", "[content]") {
    SECTION("Test random text") {
        ContentManagerFactory test(ContentManagerType::RandomText);
        std::unique_ptr<Protocol> dummy(new Protocol());
        
        std::unique_ptr<ContentManager> text(test.createContentManager(std::move(dummy)));
        REQUIRE(text->getType() == ContentManagerType::RandomText);
        // std::this_thread::sleep_for(std::chrono::milliseconds(500)); // This sleep is needed to allow the thread to exit before dummy is dtor'd
    }
}