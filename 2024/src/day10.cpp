/******************************************************************************
 * File:        day10.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-10
 *****************************************************************************/

#include "day10.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    auto island = aoc::day10::IslandMap::read(infile);

    std::cout << island.trailhead_scores<aoc::PART_1>() << "\n";
    std::cout << island.trailhead_scores<aoc::PART_2>() << "\n";

    return 0;
}
