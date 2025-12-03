/******************************************************************************
 * File:        day03.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-12-03
 *****************************************************************************/

#include "day03.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv).infile;

    auto banks = aoc::day03::read_input(infile);

    int part1 = 0;
    for (const auto &bank : banks) {
        part1 += aoc::day03::max_joltage(bank);
    }
    std::cout << part1 << "\n";

    return 0;
}
