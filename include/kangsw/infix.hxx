#pragma once

namespace kangsw::infix {
template <typename Op_>
struct base {
    using operation_type = Op_;
};

template <typename Op_, typename Lhs_>
struct infix_right_t__ {
    using lhs_type = Lhs_;
    lhs_type const* l_ = nullptr;
};

template <typename Op_, typename Lhs_>
decltype(auto) operator/(Lhs_ const& lhs, base<Op_> const& op)
{
    infix_right_t__<Op_, Lhs_> r;
    r.l_ = &lhs;

    return r;
}

template <typename Op_, typename Lhs_, typename Rhs_>
// requires Op_::is_invocable_v<Op_, Lhs_, Rhs_>
decltype(auto) operator/(infix_right_t__<Op_, Lhs_> const& op, Rhs_ const& rhs)
{
    return Op_{}(*op.l_, rhs);
}
} // namespace kangsw::infix

#define CUSTOM_INFIX_DEFINE_NEW_OPERATOR(NAME, LVAR, RVAR)               \
    struct NAME##_t : kangsw::infix::base<NAME##_t> {                    \
        template <typename L_, typename R_>                              \
        decltype(auto) operator()(L_ const& LVAR, R_ const& RVAR) const; \
    };                                                                   \
                                                                         \
    static inline NAME##_t NAME;                                         \
    template <typename L_, typename R_>                                  \
    decltype(auto) NAME##_t::operator()(L_ const& LVAR, R_ const& RVAR) const

#define CUSTOM_INFIX_DEFINE_OPERATOR_DIVIDER(NAME, LDIVIDER, RDIVIDER)           \
    struct NAME##_t;                                                             \
    template <typename Lhs_>                                                     \
    decltype(auto) operator##LDIVIDER(                                           \
      Lhs_ const& lhs, kangsw::infix::base<NAME##_t> const& op)                  \
    {                                                                            \
        kangsw::infix::infix_right_t__<NAME##_t, Lhs_> r;                        \
        r.l_ = &lhs;                                                             \
                                                                                 \
        return r;                                                                \
    }                                                                            \
                                                                                 \
    template <typename Lhs_, typename Rhs_>                                      \
    decltype(auto) operator##RDIVIDER(                                           \
      kangsw::infix::infix_right_t__<NAME##_t, Lhs_> const& op, Rhs_ const& rhs) \
    {                                                                            \
        return NAME##_t{}(*op.l_, rhs);                                          \
    }
