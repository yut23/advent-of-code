/******************************************************************************
 * File:        math.hpp
 *
 * Author:      Eric T. Johnson
 * Created:     2024-12-13
 * Description: Shared math routines for Advent of Code.
 *****************************************************************************/

#ifndef MATH_HPP_VHRQFHXN
#define MATH_HPP_VHRQFHXN

#include <array>    // for array
#include <cassert>  // for assert
#include <concepts> // for integral
#include <cstddef>  // for size_t
#include <limits>   // for numeric_limits

namespace aoc::math {

template <typename IntegerT,
          int max_digits = std::numeric_limits<IntegerT>::digits10>
constexpr std::array<IntegerT, max_digits> gen_powers_of_10() {
    std::array<IntegerT, max_digits> arr;
    IntegerT x = 1;
    for (std::size_t i = 0; i < max_digits; ++i) {
        x *= 10;
        arr[i] = x;
    }
    return arr;
}

template <std::integral IntegerT>
constexpr int num_digits(IntegerT value) {
    constexpr auto POWERS = gen_powers_of_10<IntegerT>();
    int i;
    for (i = 0; i < static_cast<int>(POWERS.size()); ++i) {
        if (POWERS[i] > value) {
            break;
        }
    }
    return i + 1;
}

template <std::integral IntegerT>
IntegerT next_power_of_10(IntegerT value) {
    constexpr auto POWERS = gen_powers_of_10<IntegerT>();
    // this array is small, so a linear search performs better
    for (const auto &pow : POWERS) {
        // cppcheck-suppress useStlAlgorithm
        if (pow > value) {
            return pow;
        }
    }
    assert(false);
}

template <typename IntegerT>
constexpr IntegerT powi(IntegerT base, unsigned int exponent) {
    if (exponent == 0) {
        return 1;
    } else if (exponent == 1) {
        return base;
    } else if (exponent == 2) {
        return base * base;
    } else {
        IntegerT tmp = powi(base, exponent / 2);
        tmp *= tmp;
        if (exponent % 2 == 1) {
            tmp *= base;
        }
        return tmp;
    }
}

} // namespace aoc::math

#endif /* end of include guard: MATH_HPP_VHRQFHXN */
