/******************************************************************************
 * File:        day12.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-12
 *****************************************************************************/

#include "day12.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    auto garden = aoc::day12::Garden::read(infile);
    std::cout << garden.fence_price<aoc::PART_1>() << "\n";
    std::cout << garden.fence_price<aoc::PART_2>() << "\n";

    return 0;
}
