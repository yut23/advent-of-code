/******************************************************************************
 * File:        day09.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-12-09
 *****************************************************************************/

#ifndef DAY09_HPP_MBAPIACN
#define DAY09_HPP_MBAPIACN

#include "lib.hpp"  // for Pos, Delta, expect_input, read_vector
#include <cstdlib>  // for abs
#include <iostream> // for istream
#include <utility>  // for move

namespace aoc {
std::istream &operator>>(std::istream &is, Pos &pos) {
    Pos tmp;
    is >> tmp.x >> aoc::expect_input(',') >> tmp.y;
    if (is) {
        pos = std::move(tmp);
    }
    return is;
}
} // namespace aoc

namespace aoc::day09 {

auto read_input(std::istream &is) { return aoc::read_vector<Pos>(is); }

long area(const Pos &p1, const Pos p2) {
    Delta diff = p2 - p1;
    return static_cast<long>(std::abs(diff.dx) + 1) * (std::abs(diff.dy) + 1);
}

} // namespace aoc::day09

#endif /* end of include guard: DAY09_HPP_MBAPIACN */
