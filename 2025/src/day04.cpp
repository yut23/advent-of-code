/******************************************************************************
 * File:        day04.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-12-04
 *****************************************************************************/

#include "day04.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv).infile;

    auto grid = aoc::day04::read_input(infile);

    auto part1 = aoc::day04::count_forklift_accessible(grid);
    std::cout << part1 << "\n";

    return 0;
}
