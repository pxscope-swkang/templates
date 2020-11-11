#pragma once
#include <execution>
#include <iterator>
#include <optional>

namespace kangsw {
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
        : count_(0)
    {
        ;
    }
    counter_base(Ty_ rhs) noexcept
        : count_(rhs)
    {
        ;
    }
    counter_base(counter_base const& rhs) noexcept
        : count_(rhs.count_)
    {
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
        : min_(min)
        , max_(max)
    {
        if (min_ > max_) {
            std::swap(min_, max_);
        }
    }

    counter_range_base(Ty_ max) noexcept
        : min_(Ty_{})
        , max_(max)
    {
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

/**
 * Executes for_each with given parallel execution policy. However, it provides current partition index within given callback.
 * It is recommended to set num_partitions as same as current thread count.
 */
template <typename It_, typename Fn_, typename ExPo_>
void for_each_partition(ExPo_&&, It_ first, It_ last, Fn_&& cb, size_t num_partitions = std::thread::hardware_concurrency())
{
    if (first == last) { throw std::invalid_argument("Zero argument"); }
    if (num_partitions == 0) { throw std::invalid_argument("Invalid partition size"); }
    size_t num_elems = std::distance(first, last);
    size_t steps = (num_elems - 1) / num_partitions + 1;
    num_partitions = std::min(num_elems, num_partitions);
    counter_range partitions(num_partitions);

    std::for_each(
      ExPo_{},
      partitions.begin(),
      partitions.end(),
      [num_elems, steps, &cb, &first](size_t partition_index) {
          It_ it = first, end;
          std::advance(it, steps * partition_index);
          std::advance(end = it, steps * (partition_index + 1) <= num_elems ? steps : num_elems - steps * partition_index);

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
void for_each_indexes(ExPo_&&, int64_t begin, int64_t end, Fn_&& cb, size_t num_partitions = std::thread::hardware_concurrency())
{
    if (begin < end) { throw std::invalid_argument("end precedes begin"); }

    counter_range range(begin, end);
    for_each_partition(ExPo_{}, range.begin(), range.end(), std::forward<Fn_>(cb), num_partitions);
}

template <typename ExPo_, typename Fn_>
void for_each_indexes(int64_t begin, int64_t end, Fn_&& cb)
{
    counter_range range(begin, end);
    std::for_each(range.begin(), range.end(), std::forward<Fn_>(cb));
}

template <typename Mutex_>
decltype(auto) lock_read(Mutex_& m)
{
    if constexpr (std::is_same<Mutex_, std::mutex>::value) {
        return std::unique_lock<Mutex_>{m};
    }
    else {
        return std::shared_lock<Mutex_>{m};
    }
}

template <typename Mutex_>
decltype(auto) lock_write(Mutex_& m)
{
    return std::unique_lock<Mutex_>{m};
}

template <typename... Args_>
std::string format_string(char const* fmt, Args_&&... args)
{
    std::string s;
    auto buflen = snprintf(nullptr, 0, fmt, std::forward<Args_>(args)...);
    s.resize(buflen);

    snprintf(s.data(), buflen, fmt, std::forward<Args_>(args)...);
    return s;
}

enum class recurse_return {
    do_continue,
    do_break
};

/**
 * 재귀적으로 작업을 수행합니다.
 * @param root 루트가 되는 노드입니다.
 * @param recurse Ty_로부터 하위 노드를 추출합니다. void(Ty_& parent, void (emplacer)(Ty_&)) 시그니쳐를 갖는 콜백으로, parent의 자손 노드를 iterate해 각각의 노드에 대해 emplacer(node)를 호출하여 재귀적인 작업을 수행할 수 있습니다.
 * 
 */
template <typename Ty_, typename Recurse_, typename Op_>
decltype(auto) recurse_for_each(Ty_&& root, Recurse_&& recurse, Op_&& op)
{
    std::vector<std::pair<Ty_&, size_t>> stack;
    stack.emplace_back(root, 0);

    while (!stack.empty()) {
        auto ref = stack.back();
        stack.pop_back();

        if constexpr (std::is_invocable_v<Op_, Ty_, size_t>) {
            if constexpr (std::is_invocable_r_v<recurse_return, Op_, Ty_, size_t>) {
                if (op(ref.first, ref.second) == recurse_return::do_break) { break; }
            }
            else {
                op(ref.first, ref.second);
            }
        }
        else {
            if constexpr (std::is_invocable_r_v<recurse_return, Op_, Ty_, size_t>) {
                if (op(ref.first) == recurse_return::do_break) { break; }
            }
            else {
                op(ref.first);
            }
        }

        auto emplacer = [&stack, n = ref.second + 1](Ty_& arg) { stack.emplace_back(arg, n); };
        recurse(ref.first, std::move(emplacer));
    }
}

} // namespace kangsw
