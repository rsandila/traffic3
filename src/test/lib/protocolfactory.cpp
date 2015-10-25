#include "catch.hpp"
#include "hippomocks.h"
#include "protocolfactory.h"

TEST_CASE("Protocol Factory", "[protocol]") {
    SECTION("TCP4") {
        ProtocolFactory factory(ProtocolType::TCP4);
        std::unique_ptr<Protocol> protocol = factory.createProtocol();
        REQUIRE(protocol.get() != nullptr);
        REQUIRE(dynamic_cast<ProtocolTCP4 *>(protocol.get()) != nullptr);
    }
}
