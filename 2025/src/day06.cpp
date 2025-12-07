/******************************************************************************
 * File:        day06.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-12-06
 *****************************************************************************/

#include "day06.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv).infile;

    auto [problems1, problems2] = aoc::day06::read_input(infile);

    long part1 = 0;
    for (const auto &problem : problems1) {
        part1 += problem.calculate();
    }
    std::cout << part1 << "\n";

    long part2 = 0;
    for (const auto &problem : problems2) {
        part2 += problem.calculate();
    }
    std::cout << part2 << "\n";

    return 0;
}
