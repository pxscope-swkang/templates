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
#include "zip.hxx"

namespace kangsw {
/**
 * @see https://stackoverflow.com/questions/55288555/c-check-if-statement-can-be-evaluated-constexpr
 * evaluates given expression can be constexpr
 */
template <class Lambda, int = (Lambda{}(), 0)>
constexpr bool is_constexpr(Lambda) { return true; }
constexpr bool is_constexpr(...) { return false; }

template <typename Ty_>
// requires std::is_arithmetic_v<Ty_>&& std::is_integral_v<Ty_>
class counter_base {
public:
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = ptrdiff_t;
    using value_type = Ty_;
    using reference = Ty_&;
    using pointer = Ty_*;

public:
    counter_base() noexcept
        :
        count_(0) {
        ;
    }
    counter_base(Ty_ rhs) noexcept
        :
        count_(rhs) {
        ;
    }
    counter_base(counter_base const& rhs) noexcept
        :
        count_(rhs.count_) {
        ;
    }

public:
    friend counter_base operator+(counter_base c, difference_type n) { return counter_base(c.count_ + n); }
    friend counter_base operator+(difference_type n, counter_base c) { return c + n; }
    friend counter_base operator-(counter_base c, difference_type n) { return counter_base(c.count_ - n); }
    friend counter_base operator-(difference_type n, counter_base c) { return c - n; }
    difference_type operator-(counter_base o) { return count_ - o.count_; }
    counter_base& operator+=(difference_type n) { return count_ += n, *this; }
    counter_base& operator-=(difference_type n) { return count_ -= n, *this; }
    counter_base& operator++() { return ++count_, *this; }
    counter_base operator++(int) { return ++count_, counter_base(count_ - 1); }
    counter_base& operator--() { return --count_, *this; }
    counter_base operator--(int) { return --count_, counter_base(count_ - 1); }
    bool operator<(counter_base o) const { return count_ < o.count_; }
    bool operator>(counter_base o) const { return count_ > o.count_; }
    bool operator==(counter_base o) const { return count_ == o.count_; }
    bool operator!=(counter_base o) const { return count_ != o.count_; }
    Ty_ const& operator*() const { return count_; }
    Ty_ const* operator->() const { return &count_; }
    Ty_ const& operator*() { return count_; }
    Ty_ const* operator->() { return &count_; }

private:
    Ty_ count_;
};

template <typename Ty_>
class counter_range_base {
public:
    counter_range_base(Ty_ min, Ty_ max) noexcept
        :
        min_(min),
        max_(max) {
        if (min_ > max_) {
            std::swap(min_, max_);
        }
    }

    counter_range_base(Ty_ max) noexcept
        :
        min_(Ty_{}),
        max_(max) {
        if (min_ > max_) {
            std::swap(min_, max_);
        }
    }

    counter_base<Ty_> begin() const { return min_; }
    counter_base<Ty_> cbegin() const { return min_; }
    counter_base<Ty_> end() const { return max_; }
    counter_base<Ty_> cend() const { return max_; }

private:
    Ty_ min_, max_;
};

using counter = counter_base<int64_t>;
using counter_range = counter_range_base<int64_t>;
using iota = counter_range_base<int64_t>;

/**
 * Executes for_each with given parallel execution policy. However, it provides current partition index within given callback.
 * It is recommended to set num_partitions as same as current thread count.
 */
template <typename It_, typename Fn_, typename ExPo_>
void for_each_partition(ExPo_&&, It_ first, It_ last, Fn_&& cb, size_t num_partitions = std::thread::hardware_concurrency()) {
    if (first == last) { throw std::invalid_argument("Zero argument"); }
    if (num_partitions == 0) { throw std::invalid_argument("Invalid partition size"); }
    size_t num_elems = std::distance(first, last);
    size_t steps = std::max<size_t>(1, num_elems / num_partitions);
    num_partitions = std::min(num_elems, num_partitions);
    counter_range partitions(num_partitions);

    std::for_each(
      ExPo_{},
      partitions.begin(),
      partitions.end(),
      [num_elems, num_partitions, steps, &cb, &first](size_t partition_index) {
          It_ it = first, end;
          size_t current_index = steps * partition_index;
          std::advance(it, current_index);
          std::advance(end = it, partition_index + 1 == num_partitions ? num_elems - current_index : steps);

          for (; it != end; ++it) {
              if constexpr (std::is_invocable_v<Fn_, decltype(*it)>) {
                  cb(*it);
              }
              else if constexpr (std::is_invocable_v<Fn_, decltype(*it), size_t /*partition*/>) {
                  cb(*it, partition_index);
              }
              else {
                  static_assert(false, "given callback has invalid signature");
              }
          }
      });
}

/**
 * convenient helper method for for_reach_partition
 */
template <typename ExPo_, typename Fn_>
void for_each_indexes(ExPo_&&, int64_t begin, int64_t end, Fn_&& cb, size_t num_partitions = std::thread::hardware_concurrency()) {
    if (begin < end) { throw std::invalid_argument("end precedes begin"); }

    counter_range range(begin, end);
    for_each_partition(ExPo_{}, range.begin(), range.end(), std::forward<Fn_>(cb), num_partitions);
}

template <typename ExPo_, typename Fn_>
void for_each_indexes(int64_t begin, int64_t end, Fn_&& cb) {
    counter_range range(begin, end);
    std::for_each(range.begin(), range.end(), std::forward<Fn_>(cb));
}

template <typename... Args_>
std::string format(char const* fmt, Args_&&... args) {
    std::string s;
    auto buflen = snprintf(nullptr, 0, fmt, std::forward<Args_>(args)...);
    s.resize(buflen);

    snprintf(s.data(), buflen, fmt, std::forward<Args_>(args)...);
    return s;
}

namespace impl__ {
enum class recurse_policy_base {
    preorder,
    postorder,
};
template <impl__::recurse_policy_base Val_>
using recurse_policy_v = std::integral_constant<impl__::recurse_policy_base, Val_>;

} // namespace impl__

namespace recurse {
constexpr impl__::recurse_policy_v<impl__::recurse_policy_base::preorder> preorder;
constexpr impl__::recurse_policy_v<impl__::recurse_policy_base::postorder> postorder;
} // namespace recurse

/**
 * 재귀적으로 작업을 수행합니다.
 * @param root 루트가 되는 노드입니다.
 * @param recurse Ty_로부터 하위 노드를 추출합니다. void(Ty_& parent, void (emplacer)(Ty_&)) 시그니쳐를 갖는 콜백으로, parent의 자손 노드를 iterate해 각각의 노드에 대해 emplacer(node)를 호출하여 재귀적인 작업을 수행할 수 있습니다.
 * 
 */
template <
  typename Ref_, typename Recurse_,
  impl__::recurse_policy_base Policy_ = impl__::recurse_policy_base::preorder>
decltype(auto) recurse_for_each(
  Ref_ root, Recurse_&& recurse,
  std::integral_constant<impl__::recurse_policy_base, Policy_> = {}) {
    if constexpr (Policy_ == impl__::recurse_policy_base::preorder) {
        std::vector<std::pair<Ref_, size_t>> stack;
        stack.emplace_back(root, 0);

        while (!stack.empty()) {
            auto ref = stack.back();
            stack.pop_back();

            if constexpr (std::is_invocable_v<Recurse_, Ref_, void(Ref_)>) {
                recurse(ref.first, [&stack, n = ref.second + 1](Ref_ arg) { stack.emplace_back(arg, n); });
            }
            else if constexpr (std::is_invocable_v<Recurse_, Ref_, size_t, void(Ref_)>) {
                recurse(ref.first, ref.second, [&stack, n = ref.second + 1](Ref_ arg) { stack.emplace_back(arg, n); });
            }
            else {
                static_assert(false);
            }
        }
    }
    else if constexpr (Policy_ == impl__::recurse_policy_base::postorder) {
        static_assert(false); // do it later
    }
    else {
        static_assert(false);
    }
}

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
} // namespace kangsw
