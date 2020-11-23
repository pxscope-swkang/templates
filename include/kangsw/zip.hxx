/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <ki6080@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.      Seungwoo Kang.
 * ----------------------------------------------------------------------------
 */
#pragma once
#include <stdexcept>
#include <tuple>

/**
 * Zip functionality
 *
 * @details
 * @code{.cpp}
    auto a = {1, 2, 3};
    auto b = {1.0, 2.0, 3.0};
    auto c = {"hello", "world", "!"};
    for(auto [num0, num1, str] : kangsw::zip(a, b, c)}{
        // zip(A, B, C) returns tuple<A::reference, B::reference, C::reference>
    }
 * @endcode 
 */
namespace kangsw {
namespace impl__ {
template <typename... Args_>
class _zip_iterator {
public:
    using tuple_type = std::tuple<Args_...>;
    using iterator_category = std::forward_iterator_tag;
    using value_type = std::tuple<decltype(*Args_{})...>;
    using reference = value_type&;
    using pointer = value_type*;

private:
    template <size_t... N_>
    value_type _deref(std::index_sequence<N_...>) const {
        return std::make_tuple(std::ref(*std::get<N_>(pack_))...);
    }

    template <size_t N_ = 0>
    bool _compare_strict(_zip_iterator const& op, bool previous) const {
        if constexpr (N_ < sizeof...(Args_)) {
            auto result = std::get<N_>(op.pack_) == std::get<N_>(pack_);
            if (result != previous) {
                throw std::invalid_argument("packed tuples has difference lengths");
            }

            return _compare_strict<N_ + 1>(op, result);
        }
        else {
            return previous;
        }
    }

public:
    bool operator==(_zip_iterator const& op) const {
        return _compare_strict(op, std::get<0>(op.pack_) == std::get<0>(pack_));
    }
    bool operator!=(_zip_iterator const& op) const { return !(*this == op); }

    _zip_iterator& operator++() {
        std::apply([](auto&&... arg) { (++arg, ...); }, pack_);
        return *this;
    }

    _zip_iterator operator++(int) {
        auto copy = *this;
        return ++*this, copy;
    }

    value_type operator*() const {
        return _deref(std::make_index_sequence<sizeof...(Args_)>{});
    }

public:
    tuple_type pack_;
};

template <typename... Args_>
class _zip_range {
public:
    using tuple_type = std::tuple<Args_...>;
    using iterator = _zip_iterator<Args_...>;
    using const_iterator = _zip_iterator<Args_...>;

    _zip_iterator<Args_...> begin() const { return {begin_}; }
    _zip_iterator<Args_...> end() const { return {end_}; }

public:
    tuple_type begin_;
    tuple_type end_;
};

template <typename Ty_, typename... Ph_>
decltype(auto) _container_size(Ty_&& container, Ph_...) {
    return std::size(container);
}

} // namespace impl__

template <typename Ty_>
decltype(auto) il(std::initializer_list<Ty_> v) { return v; }

/**
 * iterable한 컨테이너들을 하나로 묶습니다.
 */
template <typename... Containers_>
decltype(auto) zip(Containers_&&... containers) {
    auto begin = std::make_tuple(std::begin(containers)...);
    auto end = std::make_tuple(std::end(containers)...);

    impl__::_zip_range<decltype(std::begin(containers))...> zips;
    zips.begin_ = begin;
    zips.end_ = end;
    return zips;
}

} // namespace kangsw
