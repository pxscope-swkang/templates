#pragma once
#include "infix.hxx"

#define CUSTOM_INFIX_DEFINE_NEW_OPERATOR(NAME, LVAR, RVAR)                                                   \
    struct NAME##_t : kangsw::infix::base<NAME##_t> {                                                        \
        template <typename L_, typename R_> decltype(auto) operator()(L_ const& LVAR, R_ const& RVAR) const; \
    };                                                                                                       \
                                                                                                             \
    static inline NAME##_t NAME;                                                                             \
    template <typename L_, typename R_>                                                                      \
    decltype(auto) NAME##_t::operator()(L_ const& LVAR, R_ const& RVAR) const

#define CUSTOM_INFIX_DEFINE_OPERATOR_DIVIDER(NAME, DIVIDER)                                                       \
    struct NAME##_t;                                                                                     \
    template <typename Lhs_>                                                                             \
    decltype(auto) operator##DIVIDER(Lhs_ const& lhs, kangsw::infix::base<NAME##_t> const& op)           \
    {                                                                                                    \
        kangsw::infix::infix_right_t__<NAME##_t, Lhs_> r;                                                \
        r.l_ = &lhs;                                                                                     \
                                                                                                         \
        return r;                                                                                        \
    }                                                                                                    \
                                                                                                         \
    template <typename Lhs_, typename Rhs_>                                                              \
    decltype(auto) operator##DIVIDER(kangsw::infix::infix_right_t__<NAME##_t, Lhs_> op, Rhs_ const& rhs) \
    {                                                                                                    \
        return NAME##_t{}(*op.l_, rhs);                                                                  \
    }
