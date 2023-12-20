/******************************************************************************
 * File:        day19.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-19
 *****************************************************************************/

#include "day19.hpp"
#include "lib.hpp"
#include <iostream> // for cout
#include <numeric>  // for reduce
#include <vector>   // for vector

int part_1(const aoc::day19::PartCategorizer &cat,
           const std::vector<aoc::day19::Part> &parts) {
    int sum = 0;
    for (const auto &part : parts) {
        if (cat.process(part)) {
            sum += std::reduce(part.begin(), part.end());
        }
    }
    return sum;
}

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    const auto &[cat, parts] = aoc::day19::read_input(infile);

    std::cout << part_1(cat, parts) << "\n";

    return 0;
}
