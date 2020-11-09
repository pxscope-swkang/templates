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
decltype(auto) operator/(infix_right_t__<Op_, Lhs_> op, Rhs_ const& rhs)
{
    return Op_{}(*op.l_, rhs);
}
} // namespace kangsw::infix
