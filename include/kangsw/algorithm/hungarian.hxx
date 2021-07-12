/**
 * Generalized implmenetation of hungarian algorithm
 */
#pragma once
#include <algorithm>
#include <kangsw/container/ndarray.hxx>
#include <kangsw/helpers/counter.hxx>
#include <ranges>

namespace kangsw::algorithm {

using hungarian_result_t = std::vector<size_t>;

template <typename Ty_>
auto is_roughly_zero(Ty_ value) {
    if constexpr (std::is_floating_point_v<Ty_>) {
        return std::abs(value) <= (sizeof(Ty_) == 4 ? 1E-6 : sizeof(Ty_) == 8 ? 1E-12
                                                                              : 0);
    }
    else {
        return value == 0;
    }
}

/**
 * 보류 ...
 * Hungarian algorithm implementation class
 */
template <typename NumTy_, typename IsZero_>
requires(std::is_integral_v<NumTy_> || std::is_floating_point_v<NumTy_>) //
  struct hungarian_solver {
    hungarian_result_t const&
    operator()(ndarray<NumTy_, 2>&& distances, IsZero_&& is_zero) {
        if (distances.dims()[0] != distances.dims()[1]) {
            throw std::logic_error{"Given distance should be square"};
        };

        // Reset solver
        _distances = std::move(distances);
        _len = _distances.dims()[0];

        _zeros.reshape(_distances.dims());
        _line_zeros_rows.resize(_len);
        _line_zeros_cols.resize(_len);
        _line_cover_mask.reshape(_distances.dims());

        _result.clear();
        _result.resize(_len);

        // initial step
        for (size_t row = 0; row < _len; ++row) { _row_zero(row); }
        for (size_t col = 0; col < _len; ++col) { _col_zero(col); }

        while (!_is_optimal(is_zero)) {
            _step1();
        }

        _result.resize(_len);

        // fill zeros again
        for (auto& idx : counter(_distances.dims())) {
            _zeros[idx] = is_zero(_distances[idx]);
        }

        _occupation.clear();
        _occupation.resize(_len);
        _find_optimal();
        return _result;
    }

private:
    enum class line_t {
        none,
        row,
        col
    };

    auto _row_subtr(size_t row, NumTy_ val) {
        for (size_t i = 0; i < _len; i++) { _distances(row, i) -= val; }
    };
    auto _col_subtr(size_t col, NumTy_ val) {
        for (size_t i = 0; i < _len; i++) { _distances(i, col) -= val; }
    };
    auto _row_min(size_t row) const {
        NumTy_ min = std::numeric_limits<NumTy_>::max();
        for (size_t i = 0; i < _len; i++) { min = std::min(min, _distances(row, i)); }
        return min;
    };
    auto _col_min(size_t col) const {
        NumTy_ min = std::numeric_limits<NumTy_>::max();
        for (size_t i = 0; i < _len; i++) { min = std::min(min, _distances(i, col)); }
        return min;
    };
    auto _col_zero(size_t col) {
        return _col_subtr(col, _col_min(col));
    };
    auto _row_zero(size_t row) {
        return _row_subtr(row, _row_min(row));
    };
    auto _col_sum(size_t col) const {
        NumTy_ sum = {};
        for (size_t i = 0; i < _len; i++) { sum += _distances(i, col); }
        return sum;
    };
    auto _row_sum(size_t row) const {
        NumTy_ sum = {};
        for (size_t i = 0; i < _len; i++) { sum += _distances(row, i); }
        return sum;
    };

    // optimal check
    // check if we can erase all zeros GE than 'len' lines
    bool _is_optimal(IsZero_& is_zero) {
        // fill zeros
        for (auto& idx : counter(_distances.dims())) {
            _zeros[idx] = is_zero(_distances[idx]);
        }

        // calculate each line's containing zero count
        std::fill(_line_zeros_rows.begin(), _line_zeros_rows.end(), 0);
        std::fill(_line_zeros_cols.begin(), _line_zeros_cols.end(), 0);

        for (size_t idx = 0; idx < _len; idx++) {
            for (size_t row = idx, col = 0; col < _len; col++) {
                _line_zeros_rows[idx] += _zeros(row, col);
            }
            for (size_t col = idx, row = 0; row < _len; row++) {
                _line_zeros_cols[idx] += _zeros(row, col);
            }
        }

        // erase maximums from line_zeros
        _lines.clear();
        for (;;) {
            auto it_rmax = std::ranges::max_element(_line_zeros_rows);
            auto it_cmax = std::ranges::max_element(_line_zeros_cols);

            // erase given zeros covered by given line.
            // dethermines whether it is row or column, by comparing maximum values of rows/cols
            if (*it_rmax > *it_cmax) {
                auto row = it_rmax - _line_zeros_rows.begin();
                _lines.emplace_back(line_t::row, row);

                *it_rmax = 0;
                for (size_t col = 0; col < _len; ++col) {
                    if (auto& target = _zeros(row, col)) {
                        target = false;
                        _line_zeros_cols[col] -= 1;
                    }
                }
            }
            else {
                auto col = it_cmax - _line_zeros_cols.begin();
                _lines.emplace_back(line_t::col, col);

                *it_cmax = 0;
                for (size_t row = 0; row < _len; ++row) {
                    if (auto& target = _zeros(row, col)) {
                        target = false;
                        _line_zeros_rows[row] -= 1;
                    }
                }
            }

            // check if there's no zero.
            if (
              std::ranges::find(_line_zeros_rows, 1) == _line_zeros_rows.end()
              && std::ranges::find(_line_zeros_cols, 1) == _line_zeros_cols.end()) {
                break;
            }
        }

        return _lines.size() >= _len;
    }

    // modifies distance matrix
    void _step1() {
        // create line-covering mask
        std::ranges::fill(_line_cover_mask, 0);
        for (auto& line : _lines) {
            if (line.first == line_t::row) {
                for (auto col : counter(_len)) { ++_line_cover_mask(line.second, col); }
            }
            else {
                for (auto row : counter(_len)) { ++_line_cover_mask(row, line.second); }
            }
        }

        // find minimum from line-covering mask
        auto min_value = std::numeric_limits<NumTy_>::max();
        for (auto index : counter(_distances.dims())) {
            if (_line_cover_mask[index] == 0) {
                min_value = std::min(_distances[index], min_value);
            }
        }

        // discount minimum value from non-line-covered area,
        //and increase double-covered area's value
        for (auto index : counter(_distances.dims())) {
            if (_line_cover_mask[index] == 0) {
                _distances[index] -= min_value;
            }
            else if (_line_cover_mask[index] == 2) {
                _distances[index] += min_value;
            }
        }
    }

    // generate optimal result
    bool _find_optimal(size_t row = 0) {
        for (size_t col = 0; col < _len; col++) {
            if (!_zeros(row, col)) { continue; }
            if (_occupation[col]) { continue; }

            _occupation[col] = true;
            if (row + 1 == _len || _find_optimal(row + 1)) {
                _result[row] = col;
                return true;
            }
            _occupation[col] = false;
        }

        return false;
    }

private:
    struct _search_stack_entity {
        size_t row;
        size_t col;
    };

private:
    hungarian_result_t _result;
    std::vector<_search_stack_entity> _search_stack;
    ndarray<NumTy_, 2> _distances;
    ndarray<int8_t, 2> _zeros;
    std::vector<size_t> _line_zeros_rows;
    std::vector<size_t> _line_zeros_cols;
    std::vector<std::pair<line_t, size_t>> _lines;
    ndarray<int8_t, 2> _line_cover_mask;
    std::vector<bool> _occupation;
    size_t _len;
};

/**
 * Calculate hungarian pairs 
 */
template <typename NumTy_, typename IsZero_ = bool (&)(NumTy_)>
requires std::is_integral_v<NumTy_> || std::is_floating_point_v<NumTy_>
auto hungarian(ndarray<NumTy_, 2>&& distances, IsZero_&& is_zero = is_roughly_zero<NumTy_>) -> hungarian_result_t //
{
    return hungarian_solver<NumTy_, IsZero_>{}(std::move(distances), std::move(is_zero));
}

/**
 * 
 */
template <typename RangeA_, typename RangeB_, typename DistFn_, typename IsZero_>
requires std::is_arithmetic_v<typename std::ranges::range_value_t<RangeA_>> //
  && std::is_arithmetic_v<typename std::ranges::range_value_t<RangeB_>>
auto hungarian(
  RangeA_ const& range_a,
  RangeB_ const& range_b,
  DistFn_&& calc,
  IsZero_&& is_zero = is_roughly_zero<std::invoke_result_t<DistFn_, std::ranges::range_value_t<RangeA_>, std::ranges::range_value_t<RangeA_>>>) -> hungarian_result_t //
{
    auto const num_rows = std::size(range_a);
    auto const num_cols = std::size(range_b);

    using distance_type
      = std::invoke_result_t<
        DistFn_,
        std::ranges::range_value_t<RangeA_>,
        std::ranges::range_value_t<RangeB_>>;

    ndarray<distance_type, 2> dists;
    dists.reshape(num_rows, num_cols);

    for (size_t n_r = 0; auto& r : range_a) {
        for (size_t n_c = 0; auto& c : range_b) {
            dists(n_r, n_c) = calc(r, c);
            n_c++;
        }
        n_r++;
    }

    return hungarian(std::move(dists), std::move(is_zero));
}
} // namespace kangsw::algorithm