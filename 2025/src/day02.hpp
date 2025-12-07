/******************************************************************************
 * File:        day02.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-12-02
 *****************************************************************************/

#ifndef DAY02_HPP_YMBKSAFO
#define DAY02_HPP_YMBKSAFO

#include "lib.hpp"
#include "util/math.hpp" // for next_power_of_10, prev_power_of_10
#include <array>         // for array
#include <cassert>       // for assert
#include <cstdlib>       // for strtol, size_t
#include <iostream>      // for istream
#include <string>        // for string, getline

namespace aoc::day02 {

long sum_invalid_ids(const std::string &line) {
    constexpr std::array POWERS_OF_10 = aoc::math::gen_powers_of_10<long>();

    const char *ptr1 = line.c_str();
    const char *ptr2 = ptr1;
    // this const_cast is annoying
    long start = std::strtol(ptr1, const_cast<char **>(&ptr2), 10);
    std::size_t start_digits = ptr2 - ptr1;
    assert(*ptr2 == '-');
    ptr1 = ptr2 + 1;
    long end = std::strtol(ptr1, const_cast<char **>(&ptr2), 10);
    std::size_t end_digits = ptr2 - ptr1;
    if constexpr (aoc::DEBUG)
        std::cerr << "range " << start << "-" << end << ":";

    if (start_digits % 2 == 1 && end_digits % 2 == 1) {
        // all values have an odd number of digits
        if constexpr (aoc::DEBUG)
            std::cerr << "\n";
        return 0;
    }

    if (start_digits % 2 == 1) {
        start = aoc::math::next_power_of_10(start);
        ++start_digits;
        if (start > end) {
            if constexpr (aoc::DEBUG)
                std::cerr << "\n";
            return 0;
        }
    }
    long start_upper = start / POWERS_OF_10[start_digits / 2];
    long start_lower = start % POWERS_OF_10[start_digits / 2];

    if (end_digits % 2 == 1) {
        end = aoc::math::prev_power_of_10(end);
        --end_digits;
        if (start > end) {
            if constexpr (aoc::DEBUG)
                std::cerr << "\n";
            return 0;
        }
    }
    long end_upper = end / POWERS_OF_10[end_digits / 2];
    long end_lower = end % POWERS_OF_10[end_digits / 2];

    constexpr auto value_for_upper = [](long x) {
        return x * aoc::math::next_power_of_10(x) + x;
    };

    long sum = 0;
    if (start_lower <= start_upper &&
        (end_upper > start_upper || end_lower >= start_upper)) {
        auto x = value_for_upper(start_upper);
        if constexpr (aoc::DEBUG)
            std::cerr << " " << x;
        sum += x;
    }
    if (start_upper < end_upper) {
        for (long upper = start_upper + 1; upper <= end_upper - 1; ++upper) {
            auto x = value_for_upper(upper);
            if constexpr (aoc::DEBUG)
                std::cerr << " " << x;
            sum += x;
        }
        if (end_lower >= end_upper) {
            auto x = value_for_upper(end_upper);
            if constexpr (aoc::DEBUG)
                std::cerr << " " << x;
            sum += x;
        }
    }
    if constexpr (aoc::DEBUG)
        std::cerr << std::endl;

    return sum;
}

} // namespace aoc::day02

#endif /* end of include guard: DAY02_HPP_YMBKSAFO */
