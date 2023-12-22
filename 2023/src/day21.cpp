/******************************************************************************
 * File:        day21.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-21
 *****************************************************************************/

#include "day21.hpp"
#include "lib.hpp"  // for parse_args
#include <iostream> // for cout

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    auto farm = aoc::day21::Farm::read(infile);

    std::cout << farm.part_1() << "\n";

    return 0;
}
