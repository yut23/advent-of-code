/******************************************************************************
 * File:        day08.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-08
 *****************************************************************************/

#include "day08.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    aoc::day08::AntennaMap antenna_map{};
    infile >> antenna_map;

    auto antinode_positions = antenna_map.find_antinodes();
    std::cout << antinode_positions.size() << "\n";

    return 0;
}
