/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <ki6080@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.      Seungwoo Kang.
 * ----------------------------------------------------------------------------
 */
#pragma once
#include <shared_mutex>
#include <string_view>
#include <system_error>
#include <unordered_map>
#include "misc.hxx"

namespace kangsw {
class safe_string_table;

struct hash_index {
public:
    constexpr hash_index(std::string_view str) noexcept :
        hash_(impl__::fnv1a_impl(str.data(), str.data() + str.size())) {}

    template <size_t N>
    constexpr hash_index(char const (&str)[N]) noexcept :
        hash_(fnv1a(str)) {}

    constexpr hash_index(size_t value = -1) noexcept :
        hash_(value) {}

    constexpr hash_index(hash_index const&) = default;
    constexpr hash_index(hash_index&&) = default;
    constexpr hash_index& operator=(hash_index const&) = default;
    constexpr hash_index& operator=(hash_index&&) = default;

    template <size_t Hash_> hash_index static from_constant() { return hash_index(Hash_); }

public:
    constexpr operator size_t() const { return hash_; }
    constexpr bool operator==(hash_index const& o) const { return o.hash_ == hash_; }
    constexpr bool operator!=(hash_index const& o) const { return o.hash_ != hash_; }
    constexpr bool operator<(hash_index const& o) const { return o.hash_ < hash_; }

    constexpr operator bool() const { return hash_ == -1; }

public:
    size_t hash() const { return hash_; }

private:
    size_t hash_;
};

inline namespace literals {
constexpr auto operator""_hash(char const* str, size_t n) {
    return hash_index({str, n});
}

struct hash_pack {
    template <size_t N>
    constexpr hash_pack(const char (&o)[N]) :
        first(o), second(o) {}

    constexpr hash_pack(const char* o) :
        first(o), second(o) {}

    hash_index const first;
    const char* second;
};

constexpr auto operator""_hp(char const* str, size_t n) {
    return hash_pack(str);
}

} // namespace literals
} // namespace kangsw

template <>
struct std::hash<kangsw::hash_index> {
    size_t operator()(kangsw::hash_index const& i) const noexcept { return i.hash(); }
};

namespace kangsw {
/**
 * hash_index로부터 이름을 빌드하기 위한 함수성
 */
class safe_string_table {
public:
    std::string_view operator[](hash_index hash) const {
        std::shared_lock _0(lock_);
        std::string_view retval = {};
        if (auto found_it = table_.find(hash); found_it != table_.end()) {
            retval = found_it->second;
        }
        return retval;
    }

    template <typename Str_>
    std::string_view push(hash_index hash, Str_&& val) {
        // optimistically assumes there is already same hash exists
        {
            std::shared_lock _0(lock_);
            auto found_it = table_.find(hash);
            ;
            if (found_it != table_.end()) {
                return found_it->second;
            }
        }

        // if there is no existing entity, takes lock and emplace given hash.
        std::unique_lock _1(lock_);
        return table_.try_emplace(hash, std::forward<Str_>(val)).first->second;
    }

    template <size_t N>
    std::pair<hash_index, std::string_view> operator()(char const (&str)[N]) {
        hash_index hsx(str);
        return {hsx, push(hsx, str)};
    }

    std::pair<hash_index, std::string_view> operator()(hash_pack p) {
        return {p.first, push(p.first, p.second)};
    }

private:
    std::unordered_map<hash_index, std::string const> table_;
    mutable std::shared_mutex lock_;
};

} // namespace kangsw
