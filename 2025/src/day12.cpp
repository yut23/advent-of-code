/******************************************************************************
 * File:        day12.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-12-12
 *****************************************************************************/

#include "day12.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv).infile;

    auto regions = aoc::day12::read_input(infile);

    int part1 = 0;
    for (const auto &region : regions) {
        if constexpr (aoc::DEBUG) {
            std::cerr << region << "\n";
        }
        if (region.area_3x3() >= region.present_area_3x3()) {
            ++part1;
        }
    }
    std::cout << part1 << "\n";

    return 0;
}
