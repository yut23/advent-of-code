/******************************************************************************
 * File:        day24.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-24
 *****************************************************************************/

#include "day24.hpp"
#include "lib.hpp"  // for parse_args, DEBUG
#include <iostream> // for cout, cerr
#include <vector>   // for vector

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
                if constexpr (aoc::DEBUG) {
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

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    std::vector<aoc::day24::Hailstone> stones = aoc::day24::read_stones(infile);

    std::cout << part_1(stones) << "\n";

    return 0;
}
