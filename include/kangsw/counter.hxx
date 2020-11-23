#pragma once
#include <iterator>

namespace kangsw {
/**
 * iota counter iterator
 */
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
class iota {
public:
    constexpr iota(Ty_ min, Ty_ max) noexcept
        :
        min_(min),
        max_(max) {
        if (min_ > max_) {
            std::swap(min_, max_);
        }
    }

    constexpr iota(Ty_ max) noexcept
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

using counter = counter_base<ptrdiff_t>;
using counter_range = iota<ptrdiff_t>;
} // namespace kangsw