/******************************************************************************
 * File:        day07.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-07
 *****************************************************************************/

#include "day07.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    auto equations = aoc::day07::read_input(infile);

    long part_1 = 0, part_2 = 0;
    for (const auto &eqn : equations) {
        if (eqn.is_valid(false)) {
            part_1 += eqn.test_value;
            part_2 += eqn.test_value;
        } else if (eqn.is_valid(true)) {
            part_2 += eqn.test_value;
        }
    }
    std::cout << part_1 << "\n" << part_2 << "\n";

    return 0;
}
