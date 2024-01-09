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

    auto garden = aoc::day21::Garden::read(infile);

    std::cout << garden.part_1() << "\n";
    // std::cout << garden.part_2() << "\n";

    return 0;
}
