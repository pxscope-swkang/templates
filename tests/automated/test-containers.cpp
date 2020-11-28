#include "catch.hpp"
#include "kangsw/ndarray.hxx"

namespace kangsw::container_test {
TEST_CASE("ndarray") {

    ndarray<int, 3> ndr(25, 35, 2);
    REQUIRE(ndr.size() == 25 * 35 * 2);

    REQUIRE_THROWS(ndr.at(25, 0, 0));
    REQUIRE_THROWS(ndr.at(24, 35, 0));
    REQUIRE_THROWS(ndr.at(24, 34, 3));

    REQUIRE_NOTHROW(ndr(24, 35, 0));
    REQUIRE_NOTHROW(ndr(24, 34, 3));
}
} // namespace kangsw::container_test
