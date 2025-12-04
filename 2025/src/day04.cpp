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

    int part1 = aoc::day04::count_forklift_accessible<aoc::PART_1>(grid);
    std::cout << part1 << "\n";

    int part2 = 0;
    int removed;
    do {
        removed = aoc::day04::count_forklift_accessible<aoc::PART_2>(grid);
        part2 += removed;
    } while (removed > 0);
    std::cout << part2 << "\n";

    return 0;
}
