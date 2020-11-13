#include "catch.hpp"
#include "kangsw/index.hxx"
#include "kangsw/infix_macros.hxx"
#include "kangsw/misc.hxx"

namespace kangsw::misc_test {

CUSTOM_INFIX_DEFINE_OPERATOR_DIVIDER(add, *, *);
CUSTOM_INFIX_DEFINE_OPERATOR_DIVIDER(add, ^, ^);
CUSTOM_INFIX_DEFINE_OPERATOR_DIVIDER(add, /, /);
CUSTOM_INFIX_DEFINE_OPERATOR_DIVIDER(add, |, |);
CUSTOM_INFIX_DEFINE_OPERATOR_DIVIDER(add, -, -);
CUSTOM_INFIX_DEFINE_OPERATOR_DIVIDER(add, &, &);
CUSTOM_INFIX_DEFINE_OPERATOR_DIVIDER(add, <, >);
CUSTOM_INFIX_DEFINE_OPERATOR_DIVIDER(add, >, <);
CUSTOM_INFIX_DEFINE_OPERATOR_DIVIDER(add, >>, <<);
CUSTOM_INFIX_DEFINE_OPERATOR_DIVIDER(add, <<, >>);
CUSTOM_INFIX_DEFINE_OPERATOR_DIVIDER(add, %, %);
CUSTOM_INFIX_DEFINE_NEW_OPERATOR(add, l, r)
{
    return l + r;
}
#define IX_ADD % add %

struct mult : kangsw::infix::base<mult> {
    template <typename L_, typename R_>
    decltype(auto) operator()(L_ const& l, R_ const& r) const
    {
        return l * r;
    }
};

TEST_CASE("infix feature test", "[custom_infix]")
{
    REQUIRE((1 IX_ADD 2) == 3);
    REQUIRE((1 ^ add ^ 2) == 3);
    REQUIRE((1 / add / 2) == 3);
    REQUIRE((1 | add | 2) == 3);
    REQUIRE((1 - add - 2) == 3);
    REQUIRE((1 & add & 2) == 3);
    REQUIRE((1 << add >> 2 << add >> 3) == 6);
    REQUIRE((1 < add > 2 < add > 3) == 6);
    REQUIRE((1 % add % 2 % add % 3) == 6);
    REQUIRE((1 - add - 2 % add % 3) == 6);
    REQUIRE((3 / mult{} / 6) == 18);
}

namespace sk = kangsw;

TEST_CASE("packed tuple test")
{
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

TEST_CASE("constexpr hashing")
{
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

    switch (hash_index(std::string("hell, world!"))) {
    case index_b: break;
    default: FAIL("index didn't match!");
    }
}
} // namespace kangsw::misc_test