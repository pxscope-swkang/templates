#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <ranges>

#include "catch.hpp"
#include "kangsw/container/circular_queue.hxx"
#include "kangsw/container/ndarray.hxx"
#include "kangsw/helpers/counter.hxx"

namespace kangsw::container_test {
TEST_CASE("ndarray") {

    ndarray<int, 3> ndr(25, 35, 2);
    REQUIRE(ndr.size() == 25 * 35 * 2);

    REQUIRE_THROWS(ndr.at(25, 0, 0));
    REQUIRE_THROWS(ndr.at(24, 35, 0));
    REQUIRE_THROWS(ndr.at(24, 34, 3));

    auto dim = ndr.dims();
    for (auto& idx : counter(dim[0], dim[1], dim[2])) {
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

TEST_CASE("circular_queue") {
    circular_queue<int, 256> s;

    for (auto i : counter(256)) { s.push(i); }
    CHECK(s.is_full());
    CHECK(s.size() == 256);
    CHECK(s.capacity() == 256);
    CHECK(s.empty() == false);
    CHECK(s.peek() == 0);
    CHECK(s.latest() == 255);
    REQUIRE_THROWS(s.push(0));

    auto cnt = counter(256);
    CHECK(std::equal(cnt.begin(), cnt.end(), s.begin()));

    std::sort(s.begin(), s.end(), [](auto a, auto b) { return b < a; });
    auto cnt2 = rcounter(256);
    CHECK(std::equal(cnt2.begin(), cnt2.end(), s.begin()));
}
} // namespace kangsw::container_test
