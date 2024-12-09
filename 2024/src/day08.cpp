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

    std::cout << antenna_map.count_antinodes<aoc::PART_1>() << "\n";
    std::cout << antenna_map.count_antinodes<aoc::PART_2>() << "\n";

    return 0;
}
