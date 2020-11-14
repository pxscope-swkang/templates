/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <ki6080@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.      Seungwoo Kang.
 * ----------------------------------------------------------------------------
 */
#include "catch.hpp"
#include "kangsw/hash_index.hxx"
#include "kangsw/infix.hxx"
#include "kangsw/misc.hxx"

namespace kangsw::misc_test {

DEFINE_NEW_INFIX_OPR(add, *, *);
DEFINE_NEW_INFIX_OPR(add, ^, ^);
DEFINE_NEW_INFIX_OPR(add, /, /);
DEFINE_NEW_INFIX_OPR(add, |, |);
DEFINE_NEW_INFIX_OPR(add, -, -);
DEFINE_NEW_INFIX_OPR(add, &, &);
DEFINE_NEW_INFIX_OPR(add, <, >);
DEFINE_NEW_INFIX_OPR(add, >, <);
DEFINE_NEW_INFIX_OPR(add, >>, <<);
DEFINE_NEW_INFIX_OPR(add, <<, >>);
DEFINE_NEW_INFIX_OPR(add, %, %);
DEFINE_NEW_INFIX(add, l, r) {
    return l + r;
}

DEFINE_NEW_INFIX_OPR_WITH_DEST(multiply, <<, <<);
DEFINE_NEW_INFIX_OPR_WITH_DEST(multiply, >>, >>);
DEFINE_NEW_INFIX_OPR_WITH_DEST(multiply, -, -);
DEFINE_NEW_INFIX_OPR_WITH_DEST(multiply, |, |);
DEFINE_NEW_INFIX_WITH_DEST(multiply, d, l, r) {
    d = (float)l * r;
}

namespace ns {

DEFINE_NEW_INFIX_OPR_WITH_DEST(multiply, >>, >>);
DEFINE_NEW_INFIX_OPR_WITH_DEST(multiply, -, -);
DEFINE_NEW_INFIX_OPR_WITH_DEST(multiply, |, |);
DEFINE_NEW_INFIX_WITH_DEST(multiply, d, l, r) {
    d = (float)l * r;
}

} // namespace ns

TEST_CASE("infix feature test", "[custom_infix]") {
    REQUIRE((1 ^ add ^ 2) == 3);
    REQUIRE((1 / add / 2) == 3);
    REQUIRE((1 | add | 2) == 3);
    REQUIRE((1 - add - 2) == 3);
    REQUIRE((1 & add & 2) == 3);
    REQUIRE((1 << add >> 2 << add >> 3) == 6);
    REQUIRE((1 < add > 2 < add > 3) == 6);
    REQUIRE((1 % add % 2 % add % 3) == 6);
    REQUIRE((1 - add - 2 % add % 3) == 6);

    float multed;
    REQUIRE((3 >> multiply(multed) >> 4) == (3.0f * 4.0f));
    REQUIRE((3 << multiply(multed) >> 4) == (3.0f * 4.0f));
    REQUIRE((3 - multiply(multed) >> 4) == (3.0f * 4.0f));
    REQUIRE((3 - multiply(multed) - 4) == (3.0f * 4.0f));
    REQUIRE((3 | multiply(multed) | 4) == (3.0f * 4.0f));
    REQUIRE((3 | ns::multiply(multed) | 4) == (3.0f * 4.0f));
    REQUIRE((3 | ns::multiply(multed) | 4) == (3.0f * 4.0f));
    REQUIRE(multed == (3.0f * 4.0f));
}

namespace sk = kangsw;

TEST_CASE("packed tuple test") {
    auto a = {1, 1, 2};
    auto b = {2, 3, 3};
    auto sum = {3, 4, 5};
    auto tup = zip(a, b, sum);

    for (auto [_0, _1, _2] : tup) {
        REQUIRE((_0 + _1) == _2);
    }

    zip(il({1, 2, 3}));
    auto c = {1, 3, 4, 5};
    REQUIRE_THROWS(zip(a, b, c));
}

TEST_CASE("constexpr hashing") {
    switch (fnv1a("hell, world!")) {
    case fnv1a("hell, world!"):
        break;

    default:
        FAIL("hash didn't work!");
        break;
    }

    switch (fnv1a("hell, world!")) {
    case fnv1a("other str"):
        FAIL("Hash not match");
        break;
    default:;
    }

    hash_index index_a("hell, world!");
    constexpr hash_index index_b("hell, world!");

    switch (index_a) {
    case index_b: break;
    default: FAIL("index didn't work!");
    }

    std::string fr = "hell, world!";
    switch (hash_index(fr)) {
    case index_b: break;
    default: FAIL("index didn't match!");
    }

    char local[] = "hell, world!";
    hash_index sd(local);
}
} // namespace kangsw::misc_test