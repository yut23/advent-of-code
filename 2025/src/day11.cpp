/******************************************************************************
 * File:        day11.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-12-11
 *****************************************************************************/

#include "day11.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv).infile;
    using aoc::day11::DeviceMap;

    const auto device_map = DeviceMap::read(infile);

    int part1 = device_map.count_paths("you", "out");
    std::cout << part1 << "\n";

    return 0;
}
