#include "catch.hpp"
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
    REQUIRE((3 << mult{} >> 6) == 18);
}

TEST_CASE("packed tuple test")
{
    auto a = {1, 1, 2};
    auto b = {2, 3, 3};
    auto sum = {3, 4, 5};
    auto tup = zip(a, b, sum);

    for (auto [_0, _1, _2] : tup) {
        REQUIRE((_0 + _1) == _2);
    }

    auto c = {1, 3, 4, 5};
    REQUIRE_THROWS(zip(a, b, c));
}

} // namespace kangsw::misc_test