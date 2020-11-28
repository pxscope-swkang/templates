#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include "catch.hpp"
#include "kangsw/counter.hxx"
#include "kangsw/ndarray.hxx"

namespace kangsw::container_test {
TEST_CASE("ndarray") {

    ndarray<int, 3> ndr(25, 35, 2);
    REQUIRE(ndr.size() == 25 * 35 * 2);

    REQUIRE_THROWS(ndr.at(25, 0, 0));
    REQUIRE_THROWS(ndr.at(24, 35, 0));
    REQUIRE_THROWS(ndr.at(24, 34, 3));

    auto dim = ndr.dims();
    for (auto idx : counter(dim[0], dim[1], dim[2])) {
        ndr[idx] += 1;
    }

    //BENCHMARK("Index Array Access") {
    //    for (auto idx : counter(dim[0], dim[1], dim[2])) {
    //        ndr[idx] += 1;
    //    }
    //};

    //BENCHMARK("Index Access") {
    //    for (auto idx : counter(dim[0], dim[1], dim[2])) {
    //        ndr(idx[0], idx[1], idx[2]) += 1;
    //    }
    //};

    //BENCHMARK("Index Access with Check") {
    //    for (auto idx : counter(dim[0], dim[1], dim[2])) {
    //        ndr.at(idx[0], idx[1], idx[2]) += 1;
    //    }
    //};

    //BENCHMARK("Iterator Access") {
    //    for (auto& i : ndr) { i += 1; }
    //};

    auto ff = ndr;
    for (auto& elem : ff) { elem = 0x9496; }
    for (auto& idx : counter(dim)) { ndr[idx] = 0x9496; }

    for (auto [target, gt] : zip(ndr, ff)) {
        CHECK(target == gt);
    }
}
} // namespace kangsw::container_test
