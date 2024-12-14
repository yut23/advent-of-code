/******************************************************************************
 * File:        day13.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-13
 *****************************************************************************/

#include "day13.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    auto claw_machines = aoc::day13::read_input(infile);

    int part_1 = 0;
    for (const auto &claw : claw_machines) {
        if constexpr (aoc::DEBUG) {
            std::cerr << claw;
        }
        const auto &[a_presses, b_presses] = claw.min_presses();
        if (a_presses > 0 && b_presses > 0) {
            int tokens = a_presses * 3 + b_presses * 1;
            if constexpr (aoc::DEBUG) {
                std::cerr << "  winnable with " << tokens << " tokens\n";
            }
            part_1 += tokens;
        }
    }
    std::cout << part_1 << "\n";

    return 0;
}
