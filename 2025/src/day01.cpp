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
    int dial, part1, part2;

    dial = 50;
    part1 = 0;
    for (const auto rot : rotations) {
        dial += rot;
        dial %= 100;
        if (dial == 0) {
            ++part1;
        }
    }
    std::cout << part1 << "\n";

    dial = 50;
    part2 = 0;
    for (const auto rot : rotations) {
        int direction = rot < 0 ? -1 : 1;
        for (int i = 0; i < std::abs(rot); ++i) {
            dial += direction;
            dial %= 100;
            if (dial == 0) {
                ++part2;
            }
        }
    }
    std::cout << part2 << "\n";

    return 0;
}
