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
    std::ifstream infile = aoc::parse_args(argc, argv).infile;

    auto claw_machines = aoc::day13::read_input(infile);

    long part_1 = 0, part_2 = 0;
    aoc::LongDelta part_2_offset{10000000000000, 10000000000000};
    for (auto &claw : claw_machines) {
        if constexpr (aoc::DEBUG) {
            std::cerr << claw;
        }
        part_1 += claw.min_tokens();

        claw.prize += part_2_offset;
        part_2 += claw.min_tokens();
    }
    std::cout << part_1 << "\n";
    std::cout << part_2 << "\n";

    return 0;
}
