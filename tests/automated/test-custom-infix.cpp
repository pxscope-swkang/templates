#include "catch.hpp"
#include "kangsw/infix.hxx"
#include "kangsw/infix_macros.hxx"

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
    printf("HEll, world!");
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