/******************************************************************************
 * File:        day14.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-14
 *****************************************************************************/

#include "day14.hpp"
#include "lib.hpp"  // for parse_args, DEBUG
#include <iostream> // for cout, cerr

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    auto platform = aoc::day14::read_platform(infile);
    if constexpr (aoc::DEBUG) {
        std::cerr << "before tilting:\n";
        std::cerr << platform << "\n";
    }

    platform.tilt(aoc::Direction::up);
    if constexpr (aoc::DEBUG) {
        std::cerr << "after tilting:\n";
        std::cerr << platform << "\n";
    }

    std::cout << platform.calculate_load() << "\n";

    return 0;
}
