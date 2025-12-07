/******************************************************************************
 * File:        day02.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-12-02
 *****************************************************************************/

#ifndef DAY02_HPP_YMBKSAFO
#define DAY02_HPP_YMBKSAFO

#include "util/math.hpp" // for next_power_of_10, prev_power_of_10, gen_powers_of_10
#include <array>         // for array
#include <cassert>       // for assert
#include <cstdlib>       // for strtoul, size_t
#include <iostream>      // for istream, ostream
#include <set>           // for set
#include <string>        // for string, getline
#include <utility>       // for swap

namespace aoc::day02 {

using int_t = unsigned long;

struct Range {
    int_t start;
    std::size_t start_digits;
    int_t end;
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
    tmp.start = std::strtoul(ptr1, const_cast<char **>(&ptr2), 10);
    tmp.start_digits = math::num_digits(tmp.start);
    assert(*ptr2 == '-');
    tmp.end = std::strtoul(ptr2 + 1, nullptr, 10);
    tmp.end_digits = math::num_digits(tmp.end);

    if (is) {
        std::swap(range, tmp);
    }
    return is;
}

std::ostream &operator<<(std::ostream &os, const Range &range) {
    os << range.start << "-" << range.end;
    return os;
}

template <unsigned int REPEATS>
int_t repdigits(int_t upper) {
    const int_t base = aoc::math::next_power_of_10(upper);
    int_t x = upper;
    for (unsigned int i = 2; i <= REPEATS; ++i) {
        x = x * base + upper;
    }
    return x;
};

// take range by value, so we can modify it as needed
template <unsigned int REPEATS>
std::set<int_t> find_invalid_ids(Range r) {
    constexpr std::array POWERS_OF_10 = aoc::math::gen_powers_of_10<int_t>();

    std::set<int_t> ids{};
    if (r.start_digits % REPEATS != 0 &&
        r.start_digits / REPEATS == r.end_digits / REPEATS) {
        // all values have an invalid number of digits
        return ids;
    }

    while (r.start_digits % REPEATS != 0) {
        r.start = aoc::math::next_power_of_10(r.start);
        ++r.start_digits;
        if (r.start_digits > 10 || r.start > r.end) {
            return ids;
        }
    }
    const int_t start_divisor =
        POWERS_OF_10[r.start_digits / REPEATS * (REPEATS - 1)];
    int_t start_upper = r.start / start_divisor;
    int_t start_lower = r.start % start_divisor;

    while (r.end_digits % REPEATS != 0) {
        r.end = aoc::math::prev_power_of_10(r.end);
        --r.end_digits;
        if (r.end_digits == 0 || r.start > r.end) {
            return ids;
        }
    }
    const int_t end_divisor =
        POWERS_OF_10[r.end_digits / REPEATS * (REPEATS - 1)];
    int_t end_upper = r.end / end_divisor;
    int_t end_lower = r.end % end_divisor;

    int_t x = 0;
    if (start_lower <= repdigits<REPEATS - 1>(start_upper) &&
        (end_upper > start_upper ||
         end_lower >= repdigits<REPEATS - 1>(start_upper))) {
        x = repdigits<REPEATS>(start_upper);
        ids.insert(x);
    }
    if (start_upper < end_upper) {
        for (int_t upper = start_upper + 1; upper <= end_upper - 1; ++upper) {
            x = repdigits<REPEATS>(upper);
            ids.insert(x);
        }
        if (end_lower >= repdigits<REPEATS - 1>(end_upper)) {
            x = repdigits<REPEATS>(end_upper);
            ids.insert(x);
        }
    }

    return ids;
}

} // namespace aoc::day02

#endif /* end of include guard: DAY02_HPP_YMBKSAFO */
