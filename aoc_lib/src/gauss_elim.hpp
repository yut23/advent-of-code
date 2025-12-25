/******************************************************************************
 * File:        gauss_elim.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-01-30
 *****************************************************************************/

#ifndef GAUSS_ELIM_HPP_SVFEFUHA
#define GAUSS_ELIM_HPP_SVFEFUHA

#include "ds/grid.hpp"   // for Grid
#include "lib.hpp"       // for DEBUG
#include "util/util.hpp" // for always_false // IWYU pragma: keep
#include <cassert>       // for assert
#include <concepts>      // for signed_integral, floating_point
#include <cstdlib>       // for size_t, abs
#include <iostream>      // for ostream, cerr
#include <numeric>       // for gcd
#include <optional>      // for optional
#include <utility>       // for swap
#include <vector>        // for vector
// IWYU pragma: no_include <algorithm>  // for fill_n

namespace aoc::math {
struct RowPermuter {
    std::vector<std::size_t> row_order;

    explicit RowPermuter(int size) : row_order(size) {
        for (std::size_t r = 0; r < row_order.size(); ++r) {
            row_order[r] = r;
        }
    }

    RowPermuter(const RowPermuter &other) = default;
    RowPermuter &operator=(const RowPermuter &other) = default;
    RowPermuter(RowPermuter &&other) noexcept = default;
    RowPermuter &operator=(RowPermuter &&other) noexcept = default;

    void swap_rows(std::size_t i, std::size_t j) {
        if (i != j) {
            if constexpr (aoc::DEBUG) {
                std::cerr << "swapping rows " << i << " and " << j << "\n";
            }
            std::swap(row_order[i], row_order[j]);
        }
    }
    std::size_t size() const { return row_order.size(); }

    template <class T>
    T &operator()(aoc::ds::Grid<T> &grid, std::size_t r, std::size_t c) const {
        return grid[grid.get_index(c, row_order[r])];
    }
    template <class T>
    const T &operator()(const aoc::ds::Grid<T> &grid, std::size_t r,
                        std::size_t c) const {
        return grid[grid.get_index(c, row_order[r])];
    }
    template <class T>
    T &operator()(std::vector<T> &vec, std::size_t r) const {
        return vec[row_order[r]];
    }
    template <class T>
    const T &operator()(const std::vector<T> &vec, std::size_t r) const {
        return vec[row_order[r]];
    }

    template <class T>
    void pretty_print(std::ostream &os, const aoc::ds::Grid<T> &grid,
                      const std::vector<T> &vec) const {
        auto field_width = os.width(0);
        for (int r = 0; r < grid.height; ++r) {
            auto row = *(grid.begin() + row_order[r]);
            os << (r == 0 ? '[' : ' ') << '[';
            for (int c = 0; const auto &value : row) {
                if (c != 0) {
                    os << " ";
                }
                os << std::setw(field_width) << pretty_print::repr(value);
                ++c;
            }
            os << " | " << std::setw(field_width)
               << pretty_print::repr(vec[row_order[r]]);
            os << ']' << (r == grid.height - 1 ? "]" : ",\n");
        }
        os << "\n";
    }
    template <class T>
    void pretty_print(std::ostream &os, const aoc::ds::Grid<T> &grid) const {
        auto field_width = os.width(0);
        for (int r = 0; r < grid.height; ++r) {
            auto row = *(grid.begin() + row_order[r]);
            os << (r == 0 ? '[' : ' ') << '[';
            for (int c = 0; const auto &value : row) {
                if (c != 0) {
                    os << " ";
                }
                os << std::setw(field_width) << pretty_print::repr(value);
                ++c;
            }
            os << ']' << (r == grid.height - 1 ? "]" : ",\n");
        }
        os << "\n";
    }

    template <class T>
    void reorder(aoc::ds::Grid<T> &grid) const {
        assert(grid.height == row_order.size());
        aoc::ds::Grid<T> tmp = aoc::ds::Grid<T>(grid.width, grid.height);
        for (std::size_t r = 0; r < row_order.size(); ++r) {
            for (int c = 0; c < grid.width; ++c) {
                tmp.at(c, r) = grid.at(c, row_order[r]);
            }
        }
        grid = std::move(tmp);
    }
    template <class T>
    void reorder(std::vector<T> &vec) const {
        assert(vec.size() == row_order.size());
        std::vector<T> tmp(vec.size());
        for (std::size_t i = 0; i < row_order.size(); ++i) {
            tmp[i] = vec[row_order[i]];
        }
        vec = std::move(tmp);
    }
};

namespace detail {
/**
 * Partial pivot at row i, swapping with the row with the maximum absolute value
 * in that column.
 *
 * Returns false if no non-zero pivot was found.
 */
template <class T>
bool partial_pivot_max(const aoc::ds::Grid<T> &A, RowPermuter &rp,
                       std::size_t i) {
    std::size_t pivot = i;
    T max_value = std::abs(rp(A, i, i));
    for (std::size_t j = i; j < rp.size(); ++j) {
        T curr_value = std::abs(rp(A, j, i));
        if (curr_value > max_value) {
            max_value = curr_value;
            pivot = j;
        }
    }
    if (max_value == 0) {
        return false;
    }
    rp.swap_rows(i, pivot);
    return true;
}

/**
 * Partial pivot at column `col`, swapping with the first row with a non-zero
 * value past `row`.
 *
 * Returns false if no non-zero pivot was found.
 */
template <class T>
bool partial_pivot_first(const aoc::ds::Grid<T> &A, RowPermuter &rp,
                         std::size_t col, long row = -1) {
    if (row == -1) {
        row = col;
    }
    std::size_t pivot;
    for (pivot = row; pivot < rp.size(); ++pivot) {
        if (rp(A, pivot, col) != 0) {
            break;
        }
    }
    if (pivot == rp.size()) {
        return false;
    }
    rp.swap_rows(row, pivot);
    if constexpr (aoc::DEBUG) {
        if (col != pivot) {
            std::cerr << std::setw(2);
            rp.pretty_print(std::cerr, A);
        }
    }
    return true;
}

} // namespace detail

/**
 * Standard Gaussian elimination on floating-point numbers.
 */
template <std::floating_point T>
std::optional<RowPermuter> gauss_elim_floating_point(aoc::ds::Grid<T> &A,
                                                     std::vector<T> &b) {
    std::size_t n = A.height;
    assert(b.size() == n);
    assert(A.width == static_cast<int>(n));
    RowPermuter rp(n);

    for (std::size_t i = 0; i < n - 1; ++i) {
        if (!detail::partial_pivot_max(A, rp, i)) {
            return {};
        }
        for (std::size_t j = i + 1; j < n; ++j) {
            T m = rp(A, j, i) / rp(A, i, i);
            rp(A, j, i) = 0;
            rp(b, j) -= m * rp(b, i);
            for (std::size_t k = i + 1; k < n; ++k) {
                rp(A, j, k) -= m * rp(A, i, k);
            }
        }
    }

    return rp;
}

/**
 * Fraction-free Gaussian elimination with partial pivoting, following Turner
 * 1995, "A simplified fraction-free integer Gauss elimination algorithm"
 * (https://apps.dtic.mil/sti/citations/ADA313755)
 */
template <std::signed_integral T>
std::optional<RowPermuter> gauss_elim_fraction_free(aoc::ds::Grid<T> &A,
                                                    std::vector<T> &b) {
    std::size_t n = A.height;
    assert(b.size() == n);
    assert(A.width == static_cast<int>(n));
    RowPermuter rp(n);

    for (std::size_t i = 0; i < n; ++i) {
        if (!detail::partial_pivot_first(A, rp, i)) {
            return {};
        }
        const T &Aii = rp(A, i, i);
        for (std::size_t j = i + 1; j < n; ++j) {
            T &Aji = rp(A, j, i);
            rp(b, j) = Aii * rp(b, j) - Aji * rp(b, i);
            for (std::size_t k = i + 1; k < n; ++k) {
                rp(A, j, k) = Aii * rp(A, j, k) - Aji * rp(A, i, k);
            }
            Aji = 0;
        }
        // remove known common factor: previous diagonal
        if (i >= 1) {
            const T &denom = rp(A, i - 1, i - 1);
            for (std::size_t j = i + 1; j < n; ++j) {
                rp(b, j) /= denom;
                for (std::size_t k = i + 1; k < n; ++k) {
                    rp(A, j, k) /= denom;
                }
            }
        }
        // remove all common factors from row i
        T factor = rp(b, i);
        for (std::size_t j = i; j < n; ++j) {
            if (factor == 1) {
                // exit early if there are no common factors
                break;
            }
            factor = std::gcd(factor, rp(A, i, j));
        }
        if (factor > 1) {
            rp(b, i) /= factor;
            for (std::size_t j = 1; j < n; ++j) {
                rp(A, i, j) /= factor;
            }
            if constexpr (aoc::DEBUG) {
                std::cerr << "reduced row " << i << " by a common factor of "
                          << factor << "\n";
            }
        }
    }

    return rp;
}

template <class T>
std::optional<RowPermuter> gauss_elim(aoc::ds::Grid<T> &A, std::vector<T> &b) {
    if constexpr (std::signed_integral<T>) {
        return gauss_elim_fraction_free(A, b);
    } else if constexpr (std::floating_point<T>) {
        return gauss_elim_floating_point(A, b);
    } else {
        static_assert(util::always_false<T>,
                      "data type must be signed integral or floating-point");
    }
}

/**
 * Gauss-Jordan elimination on floating-point numbers.
 *
 * Based on https://www.statlect.com/matrix-algebra/Gauss-Jordan-elimination.
 */
template <std::floating_point T>
std::optional<RowPermuter> gauss_jordan_floating_point(aoc::ds::Grid<T> &A,
                                                       std::vector<T> &b) {
    int rows = A.height;
    int cols = A.width;
    assert(b.size() == static_cast<std::size_t>(rows));
    RowPermuter rp(rows);

    for (int row = 0, col = 0; row < rows && col < cols; ++row, ++col) {
        while (rp(A, row, col) == 0 &&
               !detail::partial_pivot_first(A, rp, col, row)) {
            if constexpr (aoc::DEBUG) {
                std::cerr << "skipping column " << col << "\n";
            }
            ++col;
            if (col >= cols) {
                return rp;
            }
        }
        // normalize diagonal element to 1
        assert(rp(A, row, col) != 0);
        T f = rp(A, row, col);
        if (rp(b, row) != 0) {
            rp(b, row) /= f;
        }
        for (int tmp = col; tmp < cols; ++tmp) {
            auto &x = rp(A, row, tmp);
            if (x != 0) {
                x /= f;
            }
        }
        assert(rp(A, row, col) == 1);
        for (int r = 0; r < rows; ++r) {
            if (r == row) {
                // skip current row
                continue;
            }
            f = rp(A, r, col);
            rp(A, r, col) = 0;
            rp(b, r) -= f * rp(b, row);
            for (int tmp = col + 1; tmp < cols; ++tmp) {
                rp(A, r, tmp) -= f * rp(A, row, tmp);
            }
        }
        if constexpr (aoc::DEBUG) {
            std::cerr << "row " << row << " done (col=" << col << ")\n"
                      << std::setw(2);
            rp.pretty_print(std::cerr, A, b);
        }
    }

    return rp;
}

/**
 * Fraction-free Gaussian elimination with partial pivoting, following Turner
 * 1995, "A simplified fraction-free integer Gauss elimination algorithm"
 * (https://apps.dtic.mil/sti/citations/ADA313755)
 */
template <std::signed_integral T>
std::optional<RowPermuter> gauss_jordan_fraction_free(aoc::ds::Grid<T> &A,
                                                      std::vector<T> &b) {
    int rows = A.height;
    int cols = A.width;
    assert(b.size() == static_cast<std::size_t>(rows));
    // assert(A.width == static_cast<int>(rows));
    RowPermuter rp(rows);
    std::vector<int> diag_cols;

    const auto scale_row = [&rp, &A, &b, cols](int r, T factor) {
        assert(rp(b, r) % factor == 0);
        rp(b, r) /= factor;
        for (int j = 0; j < cols; ++j) {
            assert(rp(A, r, j) % factor == 0);
            rp(A, r, j) /= factor;
        }
    };

    for (int row = 0, col = 0; row < rows && col < cols; ++row, ++col) {
        while (rp(A, row, col) == 0 &&
               !detail::partial_pivot_first(A, rp, col, row)) {
            if constexpr (aoc::DEBUG) {
                std::cerr << "skipping column " << col << "\n";
            }
            ++col;
            if (col >= cols) {
                break;
            }
        }
        if (col >= cols) {
            break;
        }
        diag_cols.push_back(col);

        // remove all common factors from row
        T factor = rp(b, row);
        for (int j = 0; j < cols; ++j) {
            if (factor == 1) {
                // exit early if there are no common factors
                break;
            }
            factor = std::gcd(factor, rp(A, row, j));
        }
        // make the diagonal entry positive
        if (rp(A, row, col) < 0) {
            factor *= -1;
        }
        if (factor != 1) {
            rp(b, row) /= factor;
            for (int j = 0; j < cols; ++j) {
                rp(A, row, j) /= factor;
            }
            if constexpr (aoc::DEBUG) {
                std::cerr << "reduced row " << row << " by a common factor of "
                          << factor << " at start\n";
            }
        }

        const T &Arc = rp(A, row, col);
        for (int j = 0; j < rows; ++j) {
            if (j == row) {
                continue;
            }
            assert(Arc != 0);
            T &Ajc = rp(A, j, col);
            if (Ajc == 0) {
                continue;
            }
            T lcm = std::lcm(Ajc, Arc);
            T r_factor = Ajc, j_factor = Arc;
            if (Arc != 0) {
                r_factor = lcm / Arc;
            }
            if (Ajc != 0) {
                j_factor = lcm / Ajc;
            }
            rp(b, j) = j_factor * rp(b, j) - r_factor * rp(b, row);
            for (int k = 0; k < cols; ++k) {
                if (k == col) {
                    continue;
                }
                rp(A, j, k) = j_factor * rp(A, j, k) - r_factor * rp(A, row, k);
            }
            Ajc = 0;
        }
        // remove all common factors from row
        factor = rp(b, row);
        for (int j = col; j < cols; ++j) {
            if (factor == 1) {
                // exit early if there are no common factors
                break;
            }
            factor = std::gcd(factor, rp(A, row, j));
        }
        // make the diagonal entry positive
        if (rp(A, row, col) < 0) {
            factor *= -1;
        }
        if (factor != 1) {
            assert(rp(b, row) % factor == 0);
            rp(b, row) /= factor;
            for (int j = 0; j < cols; ++j) {
                assert(rp(A, row, j) % factor == 0);
                rp(A, row, j) /= factor;
            }
            if constexpr (aoc::DEBUG) {
                std::cerr << "reduced row " << row << " by a common factor of "
                          << factor << " at end\n";
            }
        }
        if constexpr (aoc::DEBUG) {
            std::cerr << "row " << row << " done (col=" << col << ")\n"
                      << std::setw(3);
            rp.pretty_print(std::cerr, A, b);
        }
    }

    // re-reduce all rows
    for (int row = 0; row < static_cast<int>(diag_cols.size()); ++row) {
        // remove all common factors from row
        T factor = rp(b, row);
        for (int j = 0; j < cols; ++j) {
            if (factor == 1) {
                // exit early if there are no common factors
                break;
            }
            factor = std::gcd(factor, rp(A, row, j));
        }
        // make the diagonal entry positive
        if (rp(A, row, diag_cols.at(row)) < 0) {
            factor *= -1;
        }
        if (factor != 1) {
            scale_row(row, factor);
            if constexpr (aoc::DEBUG) {
                std::cerr << "reduced row " << row << " by a common factor of "
                          << factor << " (final)\n";
            }
        }
    }

    return rp;
}

template <class T>
std::optional<RowPermuter> gauss_jordan(aoc::ds::Grid<T> &A,
                                        std::vector<T> &b) {
    if constexpr (std::signed_integral<T>) {
        return gauss_jordan_fraction_free(A, b);
    } else if constexpr (std::floating_point<T>) {
        return gauss_jordan_floating_point(A, b);
    } else {
        static_assert(util::always_false<T>,
                      "data type must be signed integral or floating-point");
    }
}

template <class T>
std::vector<T> solve_upper_triangular(const aoc::ds::Grid<T> &A,
                                      std::vector<T> &b,
                                      const RowPermuter &rp) {
    std::size_t n = rp.size();
    assert(A.height == static_cast<int>(n));
    assert(A.width == static_cast<int>(n));
    assert(b.size() == n);
    std::vector<T> x(n, 0);
    for (std::size_t i = n; i-- > 0;) {
        for (std::size_t j = i + 1; j < n; ++j) {
            rp(b, i) -= rp(A, i, j) * x[j];
        }
        if constexpr (std::signed_integral<T>) {
            assert(rp(b, i) % rp(A, i, i) == 0);
        }
        x[i] = rp(b, i) / rp(A, i, i);
    }
    return x;
}

// instantiate templates in an anonymous namespace, so static analyzers will
// check these functions
namespace {
template <class T>
void _lint_helper_template() {
    aoc::ds::Grid<T> A(5, 5);
    std::vector<T> b(5);
    auto rp = gauss_elim(A, b);
    if (rp) {
        solve_upper_triangular(A, b, *rp);
    }
}
[[maybe_unused]] void _lint_helper() {
    _lint_helper_template<int>();
    _lint_helper_template<long long>();
    _lint_helper_template<float>();
    _lint_helper_template<double>();
    _lint_helper_template<long double>();
}
} // namespace

} // namespace aoc::math

#endif /* end of include guard: GAUSS_ELIM_HPP_SVFEFUHA */
