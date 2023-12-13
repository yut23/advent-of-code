/******************************************************************************
 * File:        day13.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-13
 *****************************************************************************/

#include "day13.hpp"
#include "lib.hpp"  // for parse_args, DEBUG
#include <iostream> // for cout, cerr

void print_grid(std::ostream &os, const aoc::day13::Grid &grid) {
    for (const auto &line : grid) {
        os << line << "\n";
    }
}

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    auto grids = aoc::day13::read_grids(infile);

    int part_1 = 0;
    for (const auto &grid : grids) {
        part_1 += 100 * aoc::day13::find_reflection(grid);
        part_1 += aoc::day13::find_reflection(aoc::day13::transpose(grid));
        if constexpr (aoc::DEBUG) {
            print_grid(std::cerr, grid);
            std::cerr << "\n";
            print_grid(std::cerr, aoc::day13::transpose(grid));
            std::cerr << "\n\n";
        }
    }

    std::cout << part_1 << "\n";

    return 0;
}
