/******************************************************************************
 * File:        day16.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-17
 *****************************************************************************/

#include "day16.hpp"
#include "lib.hpp"  // for parse_args, DEBUG
#include <iostream> // for cout, cerr

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    auto grid = aoc::day16::Grid::read(infile);

    if constexpr (aoc::DEBUG) {
        std::cerr << grid << "\n";
    }

    grid.send_beam();
    if constexpr (aoc::DEBUG) {
        grid.print_energized(std::cerr);
        std::cerr << "\n";
    }
    int part_1 = grid.count_energized();
    std::cout << part_1 << "\n";

    return 0;
}
