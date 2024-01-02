/******************************************************************************
 * File:        day24.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-24
 *****************************************************************************/

#ifndef DAY24_HPP_PKLXENFE
#define DAY24_HPP_PKLXENFE

#include "lib.hpp"  // for expect_input
#include <cmath>    // for copysign
#include <iostream> // for istream, ostream
#include <optional> // for optional
#include <utility>  // for pair, move, make_pair
#include <vector>   // for vector

namespace aoc::day24 {

struct Hailstone {
    double px, py, pz;
    double vx, vy, vz;
};

std::istream &operator>>(std::istream &is, Hailstone &stone) {
    using aoc::expect_input;
    Hailstone tmp;
    is >> tmp.px >> expect_input(",") >> tmp.py >> expect_input(",") >>
        tmp.pz >> expect_input("@") >> tmp.vx >> expect_input(",") >> tmp.vy >>
        expect_input(",") >> tmp.vz;
    if (is) {
        stone = std::move(tmp);
    }
    return is;
}

std::ostream &operator<<(std::ostream &os, const Hailstone &stone) {
    os << stone.px << ", " << stone.py << ", " << stone.pz << " @ " << stone.vx
       << ", " << stone.vy << ", " << stone.vz;
    return os;
}

std::optional<std::pair<double, double>>
find_intersection_xy(const Hailstone &a, const Hailstone &b) {
    // check if lines are parallel
    if (a.vx == 0 && b.vx == 0) {
        // lines are both vertical
        return {};
    }
    if (a.vx != 0 && b.vx != 0 && a.vy / a.vx == b.vy / b.vx) {
        // lines are parallel
        return {};
    }

    /*
     * Thanks Mathematica!
     * StringRiffle[
     *  "double\ "<>StringReplace[ToString[#,InputForm],{"\""->"","->"->"="}]<>";"&/@
     *  Simplify@First@Solve[y-#<>"py"==(#<>"vy"/#<>"vx")(x-#<>"px")&/@{"a.","b."},{x,y}],
     * "\n"]
     */
    double x = (a.py * a.vx * b.vx - a.px * a.vy * b.vx - a.vx * b.py * b.vx +
                a.vx * b.px * b.vy) /
               (-(a.vy * b.vx) + a.vx * b.vy);
    double y = (a.vy * b.py * b.vx - a.py * a.vx * b.vy + a.px * a.vy * b.vy -
                a.vy * b.px * b.vy) /
               (a.vy * b.vx - a.vx * b.vy);

    if (std::copysign(1.0, x - a.px) != std::copysign(1.0, a.vx) ||
        std::copysign(1.0, x - b.px) != std::copysign(1.0, b.vx)) {
        // intersection occurred in the past for at least one of the hailstones
        return {};
    }

    return std::make_pair(x, y);
}

std::vector<Hailstone> read_stones(std::istream &is) {
    std::vector<Hailstone> stones;
    Hailstone stone{};
    while (is >> stone) {
        stones.push_back(std::move(stone));
    }
    return stones;
}

} // namespace aoc::day24

#endif /* end of include guard: DAY24_HPP_PKLXENFE */
