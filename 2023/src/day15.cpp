/******************************************************************************
 * File:        day15.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-15
 *****************************************************************************/

#include "day15.hpp"
#include "lib.hpp"  // for parse_args, DEBUG
#include <iostream> // for cout, cerr
#include <string>   // for string
#include <vector>   // for vector

int part_1(const std::vector<std::string> &steps) {
    int value = 0;
    for (const auto &step : steps) {
        int tmp = aoc::day15::hash_algorithm(step);
        if constexpr (aoc::DEBUG) {
            std::cerr << step << " becomes " << tmp << "\n";
        }
        value += tmp;
    }
    return value;
}

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    auto steps = aoc::day15::read_steps(infile);

    std::cout << part_1(steps) << "\n";

    return 0;
}
