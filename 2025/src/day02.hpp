/******************************************************************************
 * File:        day02.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-12-02
 *****************************************************************************/

#ifndef DAY02_HPP_YMBKSAFO
#define DAY02_HPP_YMBKSAFO

#include "lib.hpp"       // for DEBUG
#include "util/math.hpp" // for next_power_of_10, prev_power_of_10, gen_powers_of_10
#include <array>         // for array
#include <cassert>       // for assert
#include <cstdlib>       // for strtol, size_t
#include <iostream>      // for istream, ostream, endl
#include <string>        // for string, getline
#include <utility>       // for swap

namespace aoc::day02 {

struct Range {
    long start;
    std::size_t start_digits;
    long end;
    std::size_t end_digits;
};

std::istream &operator>>(std::istream &is, Range &range) {
    Range tmp{};
    std::string line;
    if (!std::getline(is, line, ',')) {
        return is;
    }

    const char *ptr1 = line.c_str();
    const char *ptr2 = ptr1;
    // this const_cast is annoying, but that's how the C library was designed
    tmp.start = std::strtol(ptr1, const_cast<char **>(&ptr2), 10);
    tmp.start_digits = ptr2 - ptr1;
    assert(*ptr2 == '-');
    ptr1 = ptr2 + 1;
    tmp.end = std::strtol(ptr1, const_cast<char **>(&ptr2), 10);
    tmp.end_digits = ptr2 - ptr1;

    if (is) {
        std::swap(range, tmp);
    }
    return is;
}

std::ostream &operator<<(std::ostream &os, const Range &range) {
    os << range.start << "-" << range.end;
    return os;
}

// take range by value, so we can modify it as needed
long sum_invalid_ids(Range r) {
    constexpr std::array POWERS_OF_10 = aoc::math::gen_powers_of_10<long>();

    if (r.start_digits % 2 == 1 && r.start_digits == r.end_digits) {
        // all values have an odd number of digits
        if constexpr (aoc::DEBUG)
            std::cerr << "\n";
        return 0;
    }

    if (r.start_digits % 2 == 1) {
        r.start = aoc::math::next_power_of_10(r.start);
        ++r.start_digits;
        if (r.start > r.end) {
            if constexpr (aoc::DEBUG)
                std::cerr << "\n";
            return 0;
        }
    }
    long start_upper = r.start / POWERS_OF_10[r.start_digits / 2];
    long start_lower = r.start % POWERS_OF_10[r.start_digits / 2];

    if (r.end_digits % 2 == 1) {
        r.end = aoc::math::prev_power_of_10(r.end);
        --r.end_digits;
        if (r.start > r.end) {
            if constexpr (aoc::DEBUG)
                std::cerr << "\n";
            return 0;
        }
    }
    long end_upper = r.end / POWERS_OF_10[r.end_digits / 2];
    long end_lower = r.end % POWERS_OF_10[r.end_digits / 2];

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
