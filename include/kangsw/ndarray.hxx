#pragma once
#include <array>
#include <numeric>
#include <vector>
#include "for_each.hxx"

namespace kangsw::inline containers {
/**
 * N-dimensional array
 * Basically, wrapper of an vector
 */
template <typename Ty_, size_t Dim_ = 1>
class ndarray {
public:
    using value_type = Ty_;
    using size_type = size_t;
    using dimension_type = std::array<size_type, Dim_>;
    enum : size_t { dimension = Dim_ };

private:
    template <size_type I_, bool Check_ = false, typename T_, typename... Args_>
    size_type _reduce_index(size_type step, T_ i, Args_... args) {
        constexpr auto dim_idx = dimension - I_ - 1;
        auto index = step * i;

        if constexpr (Check_) {
            if (index >= dim_[dim_idx]) { throw std::invalid_argument("array index out of range"); }
        }

        if constexpr (sizeof...(Args_)) {
            auto next_step = dim_[dim_idx] * step;
            return index + _reduce_index<I_ - 1>(next_step, args...);
        }
        else {
            return index;
        }
    }

public:
    template <typename... Ints_>
    requires((sizeof...(Ints_) == dimension) && (std::is_integral_v<Ints_> && ...))
      ndarray(Ints_... ints) {
        if constexpr (sizeof...(Ints_)) { reshape(ints...); }
    }

    ndarray() = default;

public:
    template <typename... Values_> requires((sizeof...(Values_) == dimension) && (std::is_integral_v<Values_> && ...)) //
      void reshape(Values_... values) {
        static_assert(sizeof...(values) == dimension);
        auto value = {size_type(values)...};
        std::copy(value.begin(), value.end(), dim_.begin());
        data_.resize(std::reduce(dim_.begin(), dim_.end(), size_type(1), std::multiplies<>{}));
    }

    template <typename... Idxs_> //
    requires((sizeof...(Idxs_) == dimension) && (std::is_integral_v<Idxs_> && ...)) auto&
    operator()(Idxs_... index) {
        return data_[_reduce_index<0>(1, index...)];
    }

    template <typename... Idxs_>
    requires((sizeof...(Idxs_) == dimension) && (std::is_integral_v<Idxs_> && ...)) auto&
    operator()(Idxs_... index) const {
        return data_[_reduce_index<0>(1, index...)];
    }

    template <typename... Idxs_>
    requires((sizeof...(Idxs_) == dimension) && (std::is_integral_v<Idxs_> && ...)) auto& //
      at(Idxs_... index) {
        return data_.at(_reduce_index<0, true>(1, index...));
    }

    template <typename... Idxs_>
    requires((sizeof...(Idxs_) == dimension) && (std::is_integral_v<Idxs_> && ...)) auto& //
      at(Idxs_... index) const {
        return data_.at(_reduce_index<0, true>(1, index...));
    }

    auto begin() { return data_.begin(); }
    auto cbegin() const { return data_.cbegin(); }
    auto end() { return data_.end(); }
    auto cend() const { return data_.cend(); }

    auto size() const { return data_.size(); }
    auto dims() const { return dim_; }
    auto resrve() { data_.reserve(); }
    auto shrink_to_fit() { data_.shrink_to_fit(); }

private:
    dimension_type dim_;
    std::vector<Ty_> data_;
}; // namespace kangsw::inline containers

} // namespace kangsw::inline containers
