/******************************************************************************
 * File:        day05.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-12-05
 *****************************************************************************/

#include "day05.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv).infile;

    auto [lookup, ids] = aoc::day05::read_input(infile);

    if constexpr (aoc::DEBUG)
        std::cerr << "ranges:\n" << lookup << "\n";
    int part1 = 0;
    for (long id : ids) {
        if (lookup.is_covered(id)) {
            ++part1;
        }
    }
    std::cout << part1 << "\n";

    return 0;
}
