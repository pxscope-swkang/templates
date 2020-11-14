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
#include "misc.hxx"

namespace kangsw {
struct hash_index {
public:
    constexpr hash_index(std::string_view str)
        : hash(impl__::fnv1a_impl(str.data(), str.data() + str.size())) {}

    template <size_t N>
    constexpr hash_index(char const (&str)[N])
        : hash(fnv1a(str)) {}

public:
    constexpr operator size_t() const { return hash; }
    constexpr bool operator==(hash_index const& o) const { return o.hash == hash; }
    constexpr bool operator!=(hash_index const& o) const { return o.hash != hash; }
    constexpr bool operator<(hash_index const& o) const { return o.hash < hash; }

public:
    size_t const hash;
};

/**
 * hash_index로부터 이름을 빌드하기 위한 함수성
 */
class safe_string_table {
    std::string_view operator[](hash_index hash) const {
        std::shared_lock _0(lock_);
        std::string_view retval = {};
        if (auto found_it = table_.find(hash); found_it != table_.end()) {
            retval = found_it->second;
        }
        return retval;
    }

    void push(hash_index hash, std::string val) {
        // optimistically assumes there is already same hash exists
        {
            std::shared_lock _0(lock_);
            auto found_it = table_.find(hash);
            ;
            if (found_it != table_.end()) {
                return;
            }
        }

        // if there is no existing entity, takes lock and emplace given hash.
        std::unique_lock _1(lock_);
        table_.try_emplace(hash, std::move(val));
    }

private:
    std::unordered_map<hash_index, std::string const> table_;
    std::shared_mutex lock_;
};

} // namespace kangsw

template <>
struct std::hash<kangsw::hash_index> {
    size_t operator()(kangsw::hash_index const& i) const noexcept { return i.hash; }
};