/******************************************************************************
 * File:        day10.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-10
 *****************************************************************************/

// sed 's/|/│/g; s/-/─/g; s/L/└/g; s/J/┘/g; s/7/┐/g; s/F/┌/g; s/\./ /g'

#include "day10.hpp"
#include "lib.hpp"  // for parse_args
#include <iostream> // for cout

int part_1(aoc::day10::PipeGrid grid) {
    int count = 0;
    auto it = grid.begin();
    do {
        ++it;
        ++count;
    } while (*it != grid.start_pos);
    return count / 2;
}

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    auto grid = aoc::day10::read_pipes(infile);

    std::cout << part_1(grid) << "\n";

    return 0;
}
