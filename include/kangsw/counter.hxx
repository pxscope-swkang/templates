#pragma once
#include <array>
#include <iterator>
#include <numeric>
#include "details/tuple_for_each.hxx"

namespace kangsw::inline counters {

template <typename Ty_, size_t Dim_ = 1>
// requires std::is_arithmetic_v<Ty_>&& std::is_integral_v<Ty_>
class _counter;

/**
 * iota counter iterator
 */
template <typename Ty_>
// requires std::is_arithmetic_v<Ty_>&& std::is_integral_v<Ty_>
class _counter<Ty_, 1> {
public:
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = ptrdiff_t;
    using value_type = Ty_;
    using reference = Ty_&;
    using pointer = Ty_*;

public:
    _counter() noexcept
        :
        count_(0) {
        ;
    }
    _counter(Ty_ rhs) noexcept
        :
        count_(rhs) {
        ;
    }
    _counter(_counter const& rhs) noexcept
        :
        count_(rhs.count_) {
        ;
    }

public:
    friend _counter operator+(_counter c, difference_type n) { return _counter(c.count_ + n); }
    friend _counter operator+(difference_type n, _counter c) { return c + n; }
    friend _counter operator-(_counter c, difference_type n) { return _counter(c.count_ - n); }
    friend _counter operator-(difference_type n, _counter c) { return c - n; }
    difference_type operator-(_counter o) { return count_ - o.count_; }
    _counter& operator+=(difference_type n) { return count_ += n, *this; }
    _counter& operator-=(difference_type n) { return count_ -= n, *this; }
    _counter& operator++() { return ++count_, *this; }
    _counter operator++(int) { return ++count_, _counter(count_ - 1); }
    _counter& operator--() { return --count_, *this; }
    _counter operator--(int) { return --count_, _counter(count_ - 1); }
    bool operator<(_counter o) const { return count_ < o.count_; }
    bool operator>(_counter o) const { return count_ > o.count_; }
    bool operator==(_counter o) const { return count_ == o.count_; }
    bool operator!=(_counter o) const { return count_ != o.count_; }
    Ty_ const& operator*() const { return count_; }
    Ty_ const* operator->() const { return &count_; }
    Ty_ const& operator*() { return count_; }
    Ty_ const* operator->() { return &count_; }

private:
    Ty_ count_;
};

/**
 * iota counter iterator
 */
template <typename Ty_>
// requires std::is_arithmetic_v<Ty_>&& std::is_integral_v<Ty_>
class _counter<Ty_, -1> {
public:
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = ptrdiff_t;
    using value_type = Ty_;
    using reference = Ty_&;
    using pointer = Ty_*;

public:
    _counter() noexcept
        :
        count_(0) {
        ;
    }
    _counter(Ty_ rhs) noexcept
        :
        count_(rhs) {
        ;
    }
    _counter(_counter const& rhs) noexcept
        :
        count_(rhs.count_) {
        ;
    }

public:
    friend _counter operator-(_counter c, difference_type n) { return _counter(c.count_ + n); }
    friend _counter operator-(difference_type n, _counter c) { return c + n; }
    friend _counter operator+(_counter c, difference_type n) { return _counter(c.count_ - n); }
    friend _counter operator+(difference_type n, _counter c) { return c - n; }
    difference_type operator-(_counter o) { return o.count_ - count_; }
    _counter& operator-=(difference_type n) { return count_ += n, *this; }
    _counter& operator+=(difference_type n) { return count_ -= n, *this; }
    _counter& operator--() { return ++count_, *this; }
    _counter operator--(int) { return ++count_, _counter(count_ - 1); }
    _counter& operator++() { return --count_, *this; }
    _counter operator++(int) { return --count_, _counter(count_ - 1); }
    bool operator>(_counter o) const { return count_ < o.count_; }
    bool operator<(_counter o) const { return count_ > o.count_; }
    bool operator==(_counter o) const { return count_ == o.count_; }
    bool operator!=(_counter o) const { return count_ != o.count_; }
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

    _counter<Ty_> begin() const { return min_; }
    _counter<Ty_> cbegin() const { return min_; }
    _counter<Ty_> end() const { return max_; }
    _counter<Ty_> cend() const { return max_; }

private:
    Ty_ min_, max_;
};

template <typename Ty_, size_t Dim_>
class _counter {
public:
    enum { num_dimension = Dim_ };

    using iterator_category = std::forward_iterator_tag;
    using difference_type = ptrdiff_t;
    using value_type = Ty_;
    using reference = value_type&;
    using pointer = value_type*;

    using dimension = std::array<Ty_, num_dimension>;

public:
    template <typename... Ints_>
    _counter& fetch_from(Ints_&&... ints) { return *this; }

    auto& operator[](size_t dim) const { return current[dim]; }
    auto& operator[](size_t dim) { return current[dim]; }

    template <size_t N_ = Dim_ - 1>
    void incr() {
        if constexpr (N_ == size_t(-1)) {
            current = max;
        }
        else {
            if (++current[N_] == max[N_]) {
                current[N_] = value_type{};
                incr<N_ - 1>();
            }
        }
    }

    _counter& operator++() { return incr(), *this; }
    _counter operator++(int) {
        auto cpy = *this;
        incr();
        return cpy;
    }

    bool operator==(_counter const& o) const { return current == o.current; }
    bool operator!=(_counter const& o) const { return !(*this == o); }

    auto& operator*() const { return current; }
    auto operator->() const { return &current; }
    auto& operator*() { return current; }
    auto operator->() { return &current; }

public:
    dimension max;
    dimension current;
};

template <typename SizeTy_, size_t Dim_>
struct _count_index {
    using iterator = _counter<SizeTy_, Dim_>;
    using dimension = typename iterator::dimension;
    iterator begin() const { return _counter<SizeTy_, Dim_>{max, {}}; }
    iterator end() const { return _counter<SizeTy_, Dim_>{max, max}; }

    dimension max;
};

template <typename SizeTy_>
struct _count_index<SizeTy_, 0> {};

template <typename SizeTy_>
auto counter(SizeTy_ size) {
    return iota<SizeTy_>{size};
} // namespace kangsw

template <typename SizeTy_>
auto rcounter(SizeTy_ size) {
    struct min_counter_gen {
        SizeTy_ begin_;
        SizeTy_ end_;
        _counter<SizeTy_, -1> begin() const { return {begin_}; }
        _counter<SizeTy_, -1> end() const { return {end_}; }
    };

    return min_counter_gen{.begin_ = SizeTy_(size - 1), .end_ = SizeTy_(-1)};
} // namespace kangsw

template <typename SizeTy_, typename... Ints_>
auto counter(SizeTy_ size, Ints_... args) {
    constexpr auto n_dim = sizeof...(Ints_) + 1;
    using size_type = std::decay_t<SizeTy_>;
    _count_index<size_type, n_dim> counter;
    counter.max[0] = std::forward<SizeTy_>(size);
    tuple_for_each(
      std::forward_as_tuple(std::forward<Ints_>(args)...),
      [&]<typename Int_>(Int_&& r, size_t i) { counter.max[i + 1] = std::forward<Int_>(r); });
    return counter;
}
} // namespace kangsw::inline counter