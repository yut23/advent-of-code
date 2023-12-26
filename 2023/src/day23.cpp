/******************************************************************************
 * File:        day23.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-24
 *****************************************************************************/

#include "day23.hpp"
#include "lib.hpp"
#include <iostream> // for cout

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    auto trail_map = aoc::day23::TrailMap::read(infile);

    trail_map.parse_grid();

    std::cout << trail_map.part_1() << "\n";

    return 0;
}
