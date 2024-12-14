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

namespace aoc::day13 {

struct ClawMachine {
    Pos a{0, 0};
    Pos b{0, 0};
    Pos prize{0, 0};

    std::pair<int, int> min_presses() const;
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
std::pair<int, int> ClawMachine::min_presses() const {
    const auto determinant = [](const Pos &p, const Pos &q) -> int {
        return p.x * q.y - q.x * p.y;
    };
    int denom = determinant(a, b);
    int a_numer = determinant(prize, b);
    int b_numer = determinant(a, prize);
    if constexpr (aoc::DEBUG) {
        std::cerr << "  a = " << a_numer << "/" << denom << " = "
                  << static_cast<double>(a_numer) / denom << ", b = " << b_numer
                  << "/" << denom << " = "
                  << static_cast<double>(b_numer) / denom << "\n";
    }
    if (a_numer % denom == 0 && b_numer % denom == 0) {
        return {a_numer / denom, b_numer / denom};
    }
    return {-1, -1};
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
