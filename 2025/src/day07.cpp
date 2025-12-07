/******************************************************************************
 * File:        day07.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-12-07
 *****************************************************************************/

#include "day07.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout, cerr

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv).infile;

    auto grid = aoc::day07::ManifoldGrid::read(infile);

    if constexpr (aoc::DEBUG) {
        std::cerr << grid << "\n";
    }
    int part1 = grid.propagate_tachyon_beams();
    if constexpr (aoc::DEBUG) {
        std::cerr << "after propagation:\n" << grid << "\n\n";
        if (grid.width < 20) {
            grid.print_visits(std::cerr);
        }
    }
    std::cout << part1 << "\n";
    std::cout << grid.count_timelines() << "\n";

    return 0;
}
