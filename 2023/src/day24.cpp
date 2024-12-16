/******************************************************************************
 * File:        day24.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-24
 *****************************************************************************/

#include "day24.hpp"
#include "gauss_elim.hpp" // for gauss_elim, solve_upper_triangular, RowPermuter
#include "lib.hpp"        // for parse_args, DEBUG
#include "unit_test/pretty_print.hpp"
#include <cmath>    // for llround
#include <cstdint>  // for int64_t
#include <fstream>  // for ifstream
#include <iostream> // for cout, cerr
#include <vector>   // for vector
// IWYU pragma: no_include <algorithm>  // for fill_n

constexpr long MIN_X = 200000000000000, MAX_X = 400000000000000;
constexpr long MIN_Y = 200000000000000, MAX_Y = 400000000000000;

int part_1(const std::vector<aoc::day24::Hailstone> &stones) {
    int count = 0;
    for (auto it_a = stones.cbegin(); it_a != stones.end(); ++it_a) {
        for (auto it_b = it_a + 1; it_b != stones.end(); ++it_b) {
            const auto result = aoc::day24::find_intersection_xy(*it_a, *it_b);
            if (!result) {
                continue;
            }
            const auto &[x, y] = result.value();
            if (x >= MIN_X && x <= MAX_X && y >= MIN_Y && y <= MAX_Y) {
                if constexpr (aoc::DEBUG && false) {
                    std::cerr << "Hailstone A: " << *it_a << "\n";
                    std::cerr << "Hailstone B: " << *it_b << "\n";
                    std::cerr << "Hailstones' paths will cross inside the test "
                                 "area (at x="
                              << x << ", y=" << y << ")\n\n";
                }
                ++count;
            }
        }
    }
    return count;
}

std::int64_t part_2(const std::vector<aoc::day24::Hailstone> &stones) {
    using value_t = long double;
    using namespace aoc::math;

    // keep trying different pairs of stones until one succeeds (should only
    // take a few tries)
    for (std::size_t i = 0; i < stones.size(); ++i) {
        for (std::size_t j = i + 1; j < stones.size(); ++j) {
            for (std::size_t k = 0; k < stones.size(); ++k) {
                for (std::size_t l = j + 1; l < stones.size(); ++l) {
                    auto [A, b] = aoc::day24::make_system<value_t>(
                        stones, {i, j}, {k, l});
                    if constexpr (aoc::DEBUG) {
                        std::cerr << "A:\n"
                                  << pretty_print::repr(A) << "\n"
                                  << "b: " << pretty_print::repr(b) << "\n";
                    }
                    std::optional<RowPermuter> rp = gauss_elim(A, b);
                    if constexpr (aoc::DEBUG) {
                        std::cerr << "after Gaussian elimination:\n"
                                  << "A:\n"
                                  << pretty_print::repr(A) << "\n"
                                  << "b: " << pretty_print::repr(b) << "\n";
                    }
                    if (rp) {
                        if constexpr (aoc::DEBUG) {
                            std::cerr << "row order: "
                                      << pretty_print::repr(rp->row_order)
                                      << "\n";
                        }
                        auto x = solve_upper_triangular(A, b, *rp);
                        if constexpr (aoc::DEBUG) {
                            std::cerr << "x: " << pretty_print::repr(x) << "\n";
                        }
                        return std::llround(x[0]) + std::llround(x[1]) +
                               std::llround(x[2]);
                    }
                }
            }
        }
    }
    assert(false);
}

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv).infile;

    std::vector<aoc::day24::Hailstone> stones = aoc::day24::read_stones(infile);

    std::cout << part_1(stones) << "\n";
    std::cout << part_2(stones) << "\n";

    return 0;
}
