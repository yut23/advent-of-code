/******************************************************************************
 * File:        day24.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-24
 *****************************************************************************/

#ifndef DAY24_HPP_PKLXENFE
#define DAY24_HPP_PKLXENFE

#include "ds/grid.hpp"      // for Grid
#include "lib.hpp"          // for expect_input
#include <cassert>          // for assert
#include <cmath>            // for copysign
#include <cstddef>          // for size_t
#include <initializer_list> // for initializer_list
#include <iostream>         // for istream, ostream
#include <optional>         // for optional
#include <utility>          // for pair, move, make_pair
#include <vector>           // for vector
// IWYU pragma: no_include <string>  // for operator== (expect_input)

namespace aoc::day24 {

struct Hailstone {
    long double px, py, pz;
    long double vx, vy, vz;
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

template <class T>
std::pair<aoc::ds::Grid<T>, std::vector<T>>
make_system(const std::vector<aoc::day24::Hailstone> &stones,
            const std::pair<std::size_t, std::size_t> &pair_1,
            const std::pair<std::size_t, std::size_t> &pair_2) {
    aoc::ds::Grid<T> A(6, 6, 0);
    std::vector<T> b(6, 0);
    aoc::ds::Grid<T> identity(6, 6, 0);
    const auto p = [&stones](int idx, int coord) {
        switch (coord) {
        case 0:
            return stones[idx].px;
        case 1:
            return stones[idx].py;
        case 2:
            return stones[idx].pz;
        default:
            assert(false);
        }
    };
    const auto v = [&stones](int idx, int coord) {
        switch (coord) {
        case 0:
            return stones[idx].vx;
        case 1:
            return stones[idx].vy;
        case 2:
            return stones[idx].vz;
        default:
            assert(false);
        }
    };
    // formula from https://codegolf.stackexchange.com/a/160375
    const auto eps = [](int i, int j, int k) {
        return (i - j) * (j - k) * (k - i) / 2;
    };
    for (std::size_t i = 0; const auto &[m, n] : {pair_1, pair_2}) {
        for (std::size_t j = 0; j < 3; ++j) {
            for (std::size_t k = 0; k < 3; ++k) {
                for (std::size_t l = 0; l < 3; ++l) {
                    T sign = eps(j, k, l);
                    if (sign == 0) {
                        continue;
                    }
                    b[i * 3 + j] +=
                        sign * (p(m, k) * v(m, l) - p(n, k) * v(n, l));
                    A.at(k, i * 3 + j) += sign * (v(m, l) - v(n, l));
                    A.at(l + 3, i * 3 + j) += sign * (p(m, k) - p(n, k));
                }
            }
        }
        ++i;
    }
    return {std::move(A), std::move(b)};
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
