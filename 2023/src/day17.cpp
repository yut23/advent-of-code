/******************************************************************************
 * File:        day17.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-17
 *****************************************************************************/

#include "day17.hpp"
#include "lib.hpp"  // for parse_args
#include <fstream>  // for ifstream
#include <iostream> // for cout

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    auto city_map = aoc::day17::CityMap::read(infile);

    std::cout << city_map.find_shortest_path<aoc::PART_1>() << "\n";
    std::cout << city_map.find_shortest_path<aoc::PART_2>() << "\n";

    return 0;
}
