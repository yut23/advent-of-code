/******************************************************************************
 * File:        day05.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-05
 *****************************************************************************/

#include "day05.hpp"
#include "lib.hpp"   // for parse_args, DEBUG
#include <algorithm> // for min_element
#include <iostream>  // for cerr, cout, ws

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    auto seeds = aoc::day05::read_seeds(infile);
    if constexpr (aoc::DEBUG) {
        std::cerr << "seeds:";
        for (const auto seed : seeds) {
            std::cerr << " " << seed;
        }
        std::cerr << "\n";
    }

    // skip blank line
    infile >> std::ws;

    auto results{seeds};
    while (infile) {
        auto map = aoc::day05::ConversionMap::read(infile);
        if constexpr (aoc::DEBUG) {
            std::cerr << "read map: " << map << "\n";
        }
        results = map.apply(results);
    }
    auto part_1 = *std::ranges::min_element(results);
    std::cout << part_1 << "\n";
    return 0;
}
