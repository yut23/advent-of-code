/******************************************************************************
 * File:        day19.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-19
 *****************************************************************************/

#include "day19.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv).infile;

    auto [checker, designs] = aoc::day19::read_input(infile);

    int part_1 = 0;
    long part_2 = 0;
    for (std::size_t i = 0; i < designs.size(); ++i) {
        if (long count = checker.check(designs[i])) {
            ++part_1;
            part_2 += count;
        }
    }
    std::cout << part_1 << "\n" << part_2 << "\n";

    return 0;
}
