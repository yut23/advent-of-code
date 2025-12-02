/******************************************************************************
 * File:        day01.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-12-01
 *****************************************************************************/

#include "day01.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv).infile;

    auto rotations = aoc::day01::read_input(infile);

    int dial = 50;
    int part1 = 0;
    for (const auto rot : rotations) {
        dial += rot;
        dial %= 100;
        if (dial == 0) {
            ++part1;
        }
    }

    std::cout << part1 << "\n";

    return 0;
}
