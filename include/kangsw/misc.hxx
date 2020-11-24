/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <ki6080@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.      Seungwoo Kang.
 * ----------------------------------------------------------------------------
 */
#pragma once
#include <execution>
#include <functional>
#include <iterator>
#include <optional>
#include <string>
#include <thread>
#include "counter.hxx"
#include "for_each.hxx"
#include "zip.hxx"

namespace kangsw {
/**
 * @see https://stackoverflow.com/questions/55288555/c-check-if-statement-can-be-evaluated-constexpr
 * evaluates given expression can be constexpr
 */
template <class Lambda, int = (Lambda{}(), 0)>
constexpr bool is_constexpr(Lambda) { return true; }
constexpr bool is_constexpr(...) { return false; }

/**
 * parameter pack의 N번째 argument를 얻습니다.
 */
template <size_t N, typename... Args>
decltype(auto) get_pack_element(Args&&... as) noexcept {
    return std::get<N>(std::forward_as_tuple(std::forward<Args>(as)...));
}

/**
 * 64-bit compile-time hash
 * @see https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
 */
namespace impl__ {
constexpr uint64_t fnv1a_impl(char const* s, char const* end) {
    constexpr uint64_t PRIME = 0x100000001b3;
    constexpr uint64_t OFFSET = 0xcbf29ce484222325;
    uint64_t hash = OFFSET; // magic number

    for (; s != end; ++s) { hash = (hash ^ *s) * PRIME; }
    return hash;
}
} // namespace impl__
constexpr uint64_t fnv1a(char const* str) {
    char const* h = str;
    for (; *h; ++h) {}
    return impl__::fnv1a_impl(str, h);
}

class ownership {
public:
    ownership(bool owning = true) :
        owning_(owning) {}

    ownership(ownership const&) = delete;
    ownership(ownership&& r) noexcept { *this = std::move(r); };
    ownership& operator=(ownership const&) = delete;
    ownership& operator=(ownership&& r) noexcept { return owning_ = r.owning_, r.owning_ = false, *this; };

    operator bool() { return owning_; }

private:
    bool owning_ = false;
};

template <typename... Args_>
std::string format(char const* fmt, Args_&&... args) {
    std::string s;
    auto buflen = snprintf(nullptr, 0, fmt, std::forward<Args_>(args)...);
    s.resize(buflen);

    snprintf(s.data(), buflen, fmt, std::forward<Args_>(args)...);
    return s;
}

/**
 * Executes for_each with given parallel execution policy. However, it provides current partition index within given callback.
 * It is recommended to set num_partitions as same as current thread count.
 */
template <typename It_, typename Fn_>
void for_each_threads(It_ first, It_ last, Fn_&& cb) {
    auto num_partitions = std::thread::hardware_concurrency();
    for_each_partition(std::execution::par_unseq, first, last, std::forward<Fn_>(cb), num_partitions);
}

/**
 * Executes for_each with given parallel execution policy. However, it provides current partition index within given callback.
 * It is recommended to set num_partitions as same as current thread count.
 */
template <typename Range_, typename Fn_>
void for_each_threads(Range_&& range, Fn_&& cb) {
    for_each_threads(std::begin(range), std::end(range), std::forward<Fn_>(cb));
}

} // namespace kangsw
