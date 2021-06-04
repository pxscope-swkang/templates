#include <kangsw/algorithm/hungarian.hxx>
#include "catch.hpp"

int minus(int a, int b) {
    return b - a;
}

TEST_CASE("Hungarian", "[Algorithms]") {
    kangsw::ndarray<int, 2> arr{3, 3};
    arr.assign({3, 8, 9, 4, 12, 7, 4, 8, 5});
    auto assignment = kangsw::algorithm::hungarian(std::move(arr));
    REQUIRE(std::ranges::equal(assignment, std::initializer_list{1, 0, 2}));

    arr.reshape(4, 4);
    arr.assign({10, 15, 16, 19,
                14, 13, 16, 10,
                11, 19, 18, 18,
                13, 13, 11, 12});

    assignment = kangsw::algorithm::hungarian(std::move(arr));
    REQUIRE(std::ranges::equal(assignment, std::initializer_list{0, 3, 1, 2}));
}
