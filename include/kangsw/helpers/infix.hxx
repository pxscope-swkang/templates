/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <ki6080@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.      Seungwoo Kang.
 * ----------------------------------------------------------------------------
 */
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
    base<Op_> const* op_;
};

//template <typename Op_, typename Lhs_>
//decltype(auto) operator/(Lhs_ const& lhs, base<Op_> const& op)
//{
//    infix_right_t__<Op_, Lhs_> r;
//    r.l_ = &lhs;
//    r.op_ = &op;
//
//    return r;
//}
//
//template <typename Op_, typename Lhs_, typename Rhs_>
//// requires Op_::is_invocable_v<Op_, Lhs_, Rhs_>
//decltype(auto) operator/(infix_right_t__<Op_, Lhs_> const& op, Rhs_ const& rhs)
//{
//    return (*(Op_*)op.op_)(*op.l_, rhs);
//}
} // namespace kangsw::infix

#define DEFINE_NEW_INFIX(NAME, LVAR, RVAR)                               \
    struct NAME##_t : kangsw::infix::base<NAME##_t> {                    \
        template <typename L_, typename R_>                              \
        decltype(auto) operator()(L_ const& LVAR, R_ const& RVAR) const; \
    };                                                                   \
                                                                         \
    static inline NAME##_t NAME;                                         \
    template <typename L_, typename R_>                                  \
    decltype(auto) NAME##_t::operator()(L_ const& LVAR, R_ const& RVAR) const

#define DEFINE_NEW_INFIX_WITH_DEST(NAME, DNAME, LVAR, RVAR)                  \
    template <typename DTYPE>                                                \
    struct NAME##_t : kangsw::infix::base<NAME##_t<DTYPE>> {                 \
        template <typename L_, typename R_>                                  \
        decltype(auto) operator()(L_ const& LVAR, R_ const& RVAR) const {    \
            return (*this)(DNAME, LVAR, RVAR), DNAME;                        \
        }                                                                    \
        template <typename L_, typename R_>                                  \
        void operator()(DTYPE& DNAME, L_ const& LVAR, R_ const& RVAR) const; \
        NAME##_t(DTYPE& DEST)                                                \
            : DNAME(DEST) {}                                                 \
        DTYPE& DNAME;                                                        \
    };                                                                       \
                                                                             \
    template <typename DTYPE>                                                \
    static auto NAME(DTYPE& ref) {                                           \
        return NAME##_t<DTYPE>{ref};                                         \
    }                                                                        \
    template <typename DTYPE>                                                \
    template <typename L_, typename R_>                                      \
    void NAME##_t<DTYPE>::operator()(DTYPE& DNAME, L_ const& LVAR, R_ const& RVAR) const

#define DEFINE_NEW_INFIX_OPR(NAME, LDIVIDER, RDIVIDER)                             \
    struct NAME##_t;                                                               \
    template <typename Lhs_>                                                       \
    decltype(auto) operator##LDIVIDER(                                             \
      Lhs_ const& lhs, kangsw::infix::base<NAME##_t> const& op) {                  \
        kangsw::infix::infix_right_t__<NAME##_t, Lhs_> r;                          \
        r.l_ = &lhs;                                                               \
                                                                                   \
        return r;                                                                  \
    }                                                                              \
                                                                                   \
    template <typename Lhs_, typename Rhs_>                                        \
    decltype(auto) operator##RDIVIDER(                                             \
      kangsw::infix::infix_right_t__<NAME##_t, Lhs_> const& op, Rhs_ const& rhs) { \
        return NAME##_t{}(*op.l_, rhs);                                            \
    }

#define DEFINE_NEW_INFIX_OPR_WITH_DEST(NAME, LDIVIDER, RDIVIDER)                         \
    template <typename DEST> struct NAME##_t;                                            \
    template <typename DEST, typename Lhs_>                                              \
    decltype(auto) operator##LDIVIDER(                                                   \
      Lhs_ const& lhs, kangsw::infix::base<NAME##_t<DEST>> const& op) {                  \
        kangsw::infix::infix_right_t__<NAME##_t<DEST>, Lhs_> r;                          \
        r.l_ = &lhs, r.op_ = &op;                                                        \
                                                                                         \
        return r;                                                                        \
    }                                                                                    \
                                                                                         \
    template <typename DEST, typename Lhs_, typename Rhs_>                               \
    decltype(auto) operator##RDIVIDER(                                                   \
      kangsw::infix::infix_right_t__<NAME##_t<DEST>, Lhs_> const& op, Rhs_ const& rhs) { \
        return (*(NAME##_t<DEST>*)op.op_)(*op.l_, rhs);                                  \
    }

template <typename DTYPE>
struct NAME_t : kangsw::infix::base<NAME_t<DTYPE>> {
    template <typename L_, typename R_>
    decltype(auto) operator()(L_ const& LVAR, R_ const& RVAR) const {
        return (*this)(DNAME, LVAR, RVAR), DNAME;
    }
    template <typename L_, typename R_>
    void operator()(DTYPE& DNAME, L_ const& LVAR, R_ const& RVAR) const;
    NAME_t(DTYPE& DEST)
        : DNAME(DEST) {}
    DTYPE& DNAME;
};
template <typename DTYPE>
template <typename L_, typename R_>
void NAME_t<DTYPE>::operator()(DTYPE& DNAME, L_ const& LVAR, R_ const& RVAR) const {
}

template <typename DEST> struct NAME_t;
template <typename DEST, typename Lhs_>
decltype(auto) operator/(
  Lhs_ const& lhs, kangsw::infix::base<NAME_t<DEST>> const& op) {
    kangsw::infix::infix_right_t__<NAME_t<DEST>, Lhs_> r;
    r.l_ = &lhs, r.op_ = &op;

    return r;
}

template <typename DEST, typename Lhs_, typename Rhs_>
decltype(auto) operator/(
  kangsw::infix::infix_right_t__<NAME_t<DEST>, Lhs_> const& op, Rhs_ const& rhs) {
    return (*(NAME_t<DEST>*)op.op_)(*op.l_, rhs);
}
