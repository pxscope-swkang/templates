#include "catch.hpp"
#include "kangsw/infix.hxx"
#include "kangsw/infix_macros.hxx"

CUSTOM_INFIX_DEFINE_OPERATOR_DIVIDER(add, *);
CUSTOM_INFIX_DEFINE_OPERATOR_DIVIDER(add, ^);
CUSTOM_INFIX_DEFINE_OPERATOR_DIVIDER(add, /);
CUSTOM_INFIX_DEFINE_OPERATOR_DIVIDER(add, |);
CUSTOM_INFIX_DEFINE_OPERATOR_DIVIDER(add, -);
CUSTOM_INFIX_DEFINE_OPERATOR_DIVIDER(add, &);
CUSTOM_INFIX_DEFINE_OPERATOR_DIVIDER(add, <<);
CUSTOM_INFIX_DEFINE_OPERATOR_DIVIDER(add, >>);
CUSTOM_INFIX_DEFINE_NEW_OPERATOR(add, l, r)
{
    return l + r;
}

using namespace kangsw::infix;

TEST_CASE("infix feature test", "[custom_infix]")
{
    REQUIRE((1 * add * 2) == 3);
    REQUIRE((1 ^ add ^ 2) == 3);
    REQUIRE((1 / add / 2) == 3);
    REQUIRE((1 | add | 2) == 3);
    REQUIRE((1 - add - 2) == 3);
    REQUIRE((1 & add & 2) == 3);
    REQUIRE((1 << add >> 2) == 3);
}