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
#include <unordered_map>
#include "misc.hxx"

namespace kangsw {
struct hash_index {
public:
    constexpr hash_index(std::string_view str) noexcept :
        hash(impl__::fnv1a_impl(str.data(), str.data() + str.size())) {}

    template <size_t N>
    constexpr hash_index(char const (&str)[N]) noexcept :
        hash(fnv1a(str)) {}

    // template <typename Ty_>
    // constexpr hash_index(Ty_&& v) :
    //     hash(impl__::fnv1a_impl((char const*)&v, (char const*)&v + sizeof v)) {}

public:
    constexpr operator size_t() const { return hash; }
    constexpr bool operator==(hash_index const& o) const { return o.hash == hash; }
    constexpr bool operator!=(hash_index const& o) const { return o.hash != hash; }
    constexpr bool operator<(hash_index const& o) const { return o.hash < hash; }

public:
    size_t const hash;
};

constexpr auto operator""_hash(char const* str, size_t n) {
    return hash_index({str, n});
}

} // namespace kangsw

template <>
struct std::hash<kangsw::hash_index> {
    size_t operator()(kangsw::hash_index const& i) const noexcept { return i.hash; }
};

namespace kangsw {
/**
 * hash_index로부터 이름을 빌드하기 위한 함수성
 */
class safe_string_table {
public:
    struct _hash_pack {
        template <size_t N>
        constexpr _hash_pack(char const (&str)[N]) noexcept :
            hidx_(str), str_(str) {}

    private:
        friend safe_string_table;
        hash_index hidx_;
        char const* str_;
    };

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

    std::pair<hash_index, std::string_view> operator()(_hash_pack hpack) {
        return {hpack.hidx_, push(hpack.hidx_, hpack.str_)};
    }

private:
    std::unordered_map<hash_index, std::string const> table_;
    mutable std::shared_mutex lock_;
};

} // namespace kangsw
