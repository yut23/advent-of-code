/******************************************************************************
 * File:        day07.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-12-07
 *****************************************************************************/

#include "day07.hpp"
#include "lib.hpp"
#include "unit_test/pretty_print.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv).infile;

    auto grid = aoc::day07::read_input(infile);

    if constexpr (aoc::DEBUG) {
        std::cerr << pretty_print::repr(grid) << "\n";
    }
    int part1 = aoc::day07::propagate_tachyon_beams(grid);
    if constexpr (aoc::DEBUG) {
        std::cerr << "after propagation:\n" << pretty_print::repr(grid) << "\n";
    }
    std::cout << part1 << "\n";

    return 0;
}
