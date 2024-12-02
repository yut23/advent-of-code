/******************************************************************************
 * File:        day01.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-01
 *****************************************************************************/

#include "day01.hpp"
#include "lib.hpp"
#include <algorithm> // for ranges::sort
#include <cstdlib>   // for abs, size_t
#include <iostream>  // for cout

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    auto [left, right] = aoc::day01::read_input(infile);
    std::ranges::sort(left);
    std::ranges::sort(right);

    int distance = 0;
    for (std::size_t i = 0; i < left.size(); ++i) {
        distance += std::abs(left[i] - right[i]);
    }

    std::cout << distance << "\n";

    return 0;
}
