#include <kangsw/algorithm/hungarian.hxx>
#include "catch.hpp"

int minus(int a, int b) {
    return b - a;
}

TEST_CASE("Hungarian", "[Algorithms]") {
    kangsw::ndarray<int, 2> arr{3, 3};
    arr.assign({3, 8, 9, 4, 12, 7, 4, 8, 5});
    kangsw::algorithm::hungarian(std::move(arr));
}
