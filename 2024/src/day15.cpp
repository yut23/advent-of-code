/******************************************************************************
 * File:        day15.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-16
 *****************************************************************************/

#include "day15.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv).infile;

    auto [warehouse, moves] = aoc::day15::read_input(infile);

    if constexpr (aoc::DEBUG) {
        std::cerr << "Initial state:\n" << warehouse << "\n";
    }
    for (auto dir : moves) {
        warehouse.move_robot(dir);
        if constexpr (aoc::DEBUG) {
            std::cerr << "Move " << dir << ":\n" << warehouse << "\n";
        }
    }

    std::cout << warehouse.gps_sum() << "\n";

    return 0;
}
