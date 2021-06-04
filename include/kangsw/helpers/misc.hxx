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
#include "kangsw/helpers/counter.hxx"
#include "kangsw/helpers/for_each.hxx"
#include "kangsw/helpers/trivial.hxx"
#include "kangsw/helpers/zip.hxx"

namespace kangsw {

template <typename... Args_>
std::string format(char const* fmt, Args_&&... args) {
    std::string s;
    auto buflen = snprintf(nullptr, 0, fmt, std::forward<Args_>(args)...);
    s.resize(buflen);

    snprintf(s.data(), buflen, fmt, std::forward<Args_>(args)...);
    return s;
}

inline namespace iterations {
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

} // namespace for_each
} // namespace kangsw
