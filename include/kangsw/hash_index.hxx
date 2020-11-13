/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <ki6080@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.      Seungwoo Kang.
 * ----------------------------------------------------------------------------
 */
#pragma once
#include <string_view>
#include "misc.hxx"

namespace kangsw {
struct hash_index {
public:
    hash_index(std::string_view str)
        : hash(impl__::fnv1a_impl(str.data(), str.data() + str.size()))
    {}

    constexpr hash_index(char const* str)
        : name(str)
        , hash(fnv1a(str))
    {}

public:
    constexpr operator size_t() const { return hash; }
    constexpr bool operator==(hash_index const& o) const { return o.hash == hash; }
    constexpr bool operator!=(hash_index const& o) const { return o.hash != hash; }
    constexpr bool operator<(hash_index const& o) const { return o.hash < hash; }

public:
    std::string_view const name = {};
    size_t const hash;
};

} // namespace kangsw

template <>
struct std::hash<kangsw::hash_index> {
    size_t operator()(kangsw::hash_index const& i) const noexcept { return i.hash; }
};