/******************************************************************************
 * File:        day17.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-17
 *****************************************************************************/

#include "day17.hpp"
#include "lib.hpp"  // for parse_args
#include <iostream> // for cout

int part_1(const aoc::day17::CityMap &city_map) {
    return city_map.find_shortest_path();
}

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    auto city_map = aoc::day17::CityMap::read(infile);

    std::cout << part_1(city_map) << "\n";

    return 0;
}
