/******************************************************************************
 * File:        math.hpp
 *
 * Author:      Eric T. Johnson
 * Created:     2024-12-13
 * Description: Shared math routines for Advent of Code.
 *****************************************************************************/

#ifndef MATH_HPP_VHRQFHXN
#define MATH_HPP_VHRQFHXN

#include <array>       // for array
#include <cmath>       // for signbit
#include <concepts>    // for integral, same_as
#include <cstddef>     // for size_t
#include <iterator>    // for operator==
#include <limits>      // for numeric_limits
#include <stdexcept>   // for invalid_argument
#include <type_traits> // for remove_cvref_t
#include <utility>     // for forward
#include <vector>      // for vector

namespace aoc::math {

template <typename IntegerT,
          int max_digits = std::numeric_limits<IntegerT>::digits10>
constexpr std::array<IntegerT, max_digits + 1> gen_powers_of_10() {
    std::array<IntegerT, max_digits + 1> arr;
    IntegerT x = 1;
    arr[0] = 1;
    for (std::size_t i = 1; i <= max_digits; ++i) {
        x *= 10;
        arr[i] = x;
    }
    return arr;
}

template <std::integral IntegerT>
constexpr unsigned int num_digits(IntegerT value) {
    constexpr auto POWERS = gen_powers_of_10<IntegerT>();
    unsigned int i;
    for (i = 0; i < static_cast<int>(POWERS.size()); ++i) {
        if (POWERS[i] > value) {
            break;
        }
    }
    return i;
}

/**
 * Throws std::overflow_error if the next power of 10 cannot be represented in
 * an IntegerT.
 */
template <std::integral IntegerT>
IntegerT next_power_of_10(IntegerT value) {
    constexpr auto POWERS = gen_powers_of_10<IntegerT>();
    // this array is small, so a linear search performs better
    for (IntegerT pow : POWERS) {
        // cppcheck-suppress useStlAlgorithm
        if (pow > value) {
            return pow;
        }
    }
    throw std::overflow_error("overflow in aoc::math::next_power_of_10");
}

/**
 * Returns 0 for values 0 and 1.
 */
template <std::integral IntegerT>
IntegerT prev_power_of_10(IntegerT value) {
    constexpr auto POWERS = gen_powers_of_10<IntegerT>();
    // this array is small, so a linear search performs better
    auto end = POWERS.rend();
    for (auto it = POWERS.rbegin(); it != end; ++it) {
        // cppcheck-suppress useStlAlgorithm
        if (*it < value) {
            return *it;
        }
    }
    return 0;
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

struct extended_gcd_result {
    long gcd;
    long bezout_a;
    long bezout_b;
    long quotient_a;
    long quotient_b;

    // needed for unit tests
    bool operator==(const extended_gcd_result &) const = default;
};

extended_gcd_result extended_gcd(long a, long b) {
    long old_r = a, r = b, old_s = 1, s = 0, old_t = 0, t = 1;

    constexpr auto update = [](long &old_value, long &curr_value,
                               const long quotient) {
        auto tmp = curr_value;
        curr_value = old_value - quotient * curr_value;
        old_value = tmp;
    };
    while (r != 0) {
        auto quotient = old_r / r;
        update(old_r, r, quotient);
        update(old_s, s, quotient);
        update(old_t, t, quotient);
    }

    extended_gcd_result result{.gcd = old_r,
                               .bezout_a = old_s,
                               .bezout_b = old_t,
                               .quotient_a = t,
                               .quotient_b = s};

    // fix signs
    bool gcd_sign = std::signbit(result.gcd);
    if (std::signbit(result.quotient_a) != (gcd_sign ^ std::signbit(a))) {
        // flip sign of gcd / a
        result.quotient_a = -result.quotient_a;
    }
    if (std::signbit(result.quotient_b) != (gcd_sign ^ std::signbit(b))) {
        // flip sign of gcd / b
        result.quotient_b = -result.quotient_b;
    }
    return result;
}

struct CRT {
    struct crt_entry {
        long remainder;
        long modulus;

        bool operator==(const crt_entry &) const = default;
    };

    std::vector<crt_entry> entries{};
    static crt_entry solve_pair(const crt_entry &entry_a,
                                const crt_entry &entry_b);

    void add_entry(long remainder, long modulus) {
        entries.push_back({remainder, modulus});
    }
    template <typename Entry>
        requires std::same_as<std::remove_cvref_t<Entry>, crt_entry>
    void add_entry(Entry &&entry) {
        entries.push_back(std::forward<Entry>(entry));
    }
    crt_entry solve() const;
};

CRT::crt_entry CRT::solve_pair(const crt_entry &entry_a,
                               const crt_entry &entry_b) {
    // use extended Euclidean algorithm to get the Bézout coefficients
    auto gcd = extended_gcd(entry_a.modulus, entry_b.modulus);
    if (gcd.gcd != 1) {
        throw std::invalid_argument("moduli must be coprime");
    }
    auto rem = entry_a.remainder * gcd.bezout_b * entry_b.modulus +
               entry_b.remainder * gcd.bezout_a * entry_a.modulus;
    // return the smallest positive remainder
    crt_entry entry = {.remainder = rem,
                       .modulus = entry_a.modulus * entry_b.modulus};
    while (entry.remainder < 0) {
        entry.remainder += entry.modulus;
    }
    return entry;
}

CRT::crt_entry CRT::solve() const {
    if (entries.empty()) {
        throw std::invalid_argument("CRT object must have at least 1 entry");
    }
    auto tmp = entries.front();
    for (std::size_t i = 1; i < entries.size(); ++i) {
        tmp = solve_pair(tmp, entries[i]);
    }
    return tmp;
}

} // namespace aoc::math

#endif /* end of include guard: MATH_HPP_VHRQFHXN */
