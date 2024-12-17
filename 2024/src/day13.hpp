/******************************************************************************
 * File:        day13.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-13
 *****************************************************************************/

#ifndef DAY13_HPP_WCDIDLRZ
#define DAY13_HPP_WCDIDLRZ

#include "lib.hpp"  // for Pos, expect_input, DEBUG
#include <iostream> // for istream, ostream, cerr, noskipws, ws
#include <utility>  // for pair
#include <vector>   // for vector
// IWYU pragma: no_include <string>  // for operator== (expect_input)

namespace aoc::day13 {

struct ClawMachine {
    LongPos a{0, 0};
    LongPos b{0, 0};
    LongPos prize{0, 0};

    long min_tokens() const;
};

std::istream &operator>>(std::istream &is, ClawMachine &claw) {
    ClawMachine tmp;
    is >> std::noskipws;
    is >> aoc::expect_input("Button A: X+") >> tmp.a.x >>
        aoc::expect_input(", Y+") >> tmp.a.y >> aoc::expect_input('\n') >>
        aoc::expect_input("Button B: X+") >> tmp.b.x >>
        aoc::expect_input(", Y+") >> tmp.b.y >> aoc::expect_input('\n') >>
        aoc::expect_input("Prize: X=") >> tmp.prize.x >>
        aoc::expect_input(", Y=") >> tmp.prize.y >> aoc::expect_input('\n');

    if (is) {
        // only store if the entire stream was read correctly
        claw = std::move(tmp);
    }
    return is;
}

std::ostream &operator<<(std::ostream &os, const ClawMachine &claw) {
    os << "Button A: X+" << claw.a.x << ", Y+" << claw.b.x << "\n"
       << "Button B: X+" << claw.b.x << ", Y+" << claw.b.x << "\n"
       << "Prize: X=" << claw.prize.x << ", Y=" << claw.prize.y << "\n";
    return os;
}

// this is just a 2x2 matrix solve, use Cramer's rule
long ClawMachine::min_tokens() const {
    const auto determinant = [](const LongPos &p, const LongPos &q) {
        return p.x * q.y - q.x * p.y;
    };
    long denom = determinant(a, b);
    long a_numer = determinant(prize, b);
    long b_numer = determinant(a, prize);
    if constexpr (aoc::DEBUG) {
        std::cerr << "  a = " << a_numer << "/" << denom << " = "
                  << static_cast<double>(a_numer) / denom << ", b = " << b_numer
                  << "/" << denom << " = "
                  << static_cast<double>(b_numer) / denom << "\n";
    }
    if (a_numer % denom == 0 && b_numer % denom == 0) {
        long tokens = a_numer / denom * 3 + b_numer / denom;
        if constexpr (aoc::DEBUG) {
            std::cerr << "winnable with " << a_numer / denom
                      << " A presses and " << b_numer / denom
                      << " B presses, for a total of " << tokens << " tokens\n";
        }
        return tokens;
    } else if constexpr (aoc::DEBUG) {
        std::cerr << "not winnable\n";
    }
    return 0;
}

auto read_input(std::istream &is) {
    std::vector<ClawMachine> claw_machines;
    ClawMachine claw;
    while (is >> claw) {
        claw_machines.push_back(std::move(claw));
        // consume extra newline if present
        is >> std::ws;
    }
    return claw_machines;
}

} // namespace aoc::day13

#endif /* end of include guard: DAY13_HPP_WCDIDLRZ */
