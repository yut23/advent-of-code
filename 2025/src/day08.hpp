/******************************************************************************
 * File:        day08.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-12-08
 *****************************************************************************/

#ifndef DAY08_HPP_M9FF4PKX
#define DAY08_HPP_M9FF4PKX

#include "lib.hpp"  // for Delta3, Pos3, expect_input, read_vector
#include <compare>  // for weak_ordering
#include <iostream> // for istream
#include <utility>  // for move

namespace aoc {

std::istream &operator>>(std::istream &is, Pos3 &pos) {
    Pos3 tmp;
    is >> tmp.x >> aoc::expect_input(',') >> tmp.y >> aoc::expect_input(',') >>
        tmp.z;
    if (is) {
        pos = std::move(tmp);
    }
    return is;
}

} // namespace aoc

namespace aoc::day08 {

struct JunctionBoxPair {
    Pos3 box1;
    Pos3 box2;
    long dist_sq;

    JunctionBoxPair(const Pos3 &box1, const Pos3 &box2)
        : box1(box1), box2(box2) {
        dist_sq = (box1 - box2).euclidean_distance_sq();
    }

    std::weak_ordering operator<=>(const JunctionBoxPair &other) const {
        return dist_sq <=> other.dist_sq;
    }
};

auto read_input(std::istream &is) { return aoc::read_vector<Pos3>(is); }

} // namespace aoc::day08

#endif /* end of include guard: DAY08_HPP_M9FF4PKX */
