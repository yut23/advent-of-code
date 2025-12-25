/******************************************************************************
 * File:        day10.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-12-10
 *****************************************************************************/

#include "day10.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv).infile;
    using namespace aoc::day10;

    auto machines = read_input(infile);

    int part1 = 0, part2 = 0;
    for (const Machine &machine : machines) {
        part1 += machine.min_presses<aoc::PART_1>();
        part2 += machine.min_presses<aoc::PART_2>();
    }
    std::cout << part1 << "\n";
    std::cout << part2 << "\n";

    return 0;
}
