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
        text->Start();
        REQUIRE(text->getType() == ContentManagerType::RandomText);
        text->Stop();
    }
}