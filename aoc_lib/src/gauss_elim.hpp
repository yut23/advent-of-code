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
        for (std::size_t i = 0; i < row_order.size(); ++i) {
            row_order[i] = i;
        }
    }

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
    T &operator()(aoc::ds::Grid<T> &grid, std::size_t i, std::size_t j) const {
        return grid[grid.get_index(j, row_order[i])];
    }
    template <class T>
    const T &operator()(const aoc::ds::Grid<T> &grid, std::size_t i,
                        std::size_t j) const {
        return grid[grid.get_index(j, row_order[i])];
    }
    template <class T>
    T &operator()(std::vector<T> &vec, std::size_t i) const {
        return vec[row_order[i]];
    }
    template <class T>
    const T &operator()(const std::vector<T> &vec, std::size_t i) const {
        return vec[row_order[i]];
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
        std::cerr << "warning: column " << i
                  << " is all zeros below the diagonal\n";
        return false;
    }
    rp.swap_rows(i, pivot);
    return true;
}

/**
 * Partial pivot at row i, swapping with the first row with a non-zero value.
 *
 * Returns false if no non-zero pivot was found.
 */
template <class T>
bool partial_pivot_first(const aoc::ds::Grid<T> &A, RowPermuter &rp,
                         std::size_t i) {
    std::size_t pivot;
    for (pivot = i; pivot < rp.size(); ++pivot) {
        if (rp(A, pivot, i) != 0) {
            break;
        }
    }
    if (pivot == rp.size()) {
        std::cerr << "warning: column " << i
                  << " is all zeros below the diagonal\n";
        return false;
    }
    rp.swap_rows(i, pivot);
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
