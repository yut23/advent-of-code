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

int simulate_robot(aoc::day15::Warehouse &warehouse,
                   const std::vector<aoc::AbsDirection> &moves) {
    if constexpr (aoc::DEBUG) {
        std::cerr << "Initial state:\n" << warehouse << "\n";
    }
    for (auto dir : moves) {
        warehouse.move_robot(dir);
        if constexpr (aoc::DEBUG) {
            std::cerr << "Move " << dir << ":\n" << warehouse << "\n";
        }
    }
    return warehouse.gps_sum();
}

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv).infile;

    auto [warehouse_1, moves] = aoc::day15::read_input(infile);
    // make a wider copy for part 2
    auto warehouse_2 = warehouse_1.widen();

    std::cout << simulate_robot(warehouse_1, moves) << "\n";
    std::cout << simulate_robot(warehouse_2, moves) << "\n";

    return 0;
}
