/******************************************************************************
 * File:        day12.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-12
 *****************************************************************************/

#include "day12.hpp"
#include "lib.hpp"  // for parse_args, DEBUG
#include <iostream> // for cout, cerr

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    auto records = aoc::day12::read_records(infile);

    long part_1 = 0;
    for (const auto &rec : records) {
        part_1 += rec.count_arrangements();
    }

    std::cout << part_1 << "\n";

    long part_2 = 0;
    for (const auto &rec : records) {
        part_2 += rec.repeat(5).count_arrangements();
    }

    std::cout << part_2 << "\n";

    return 0;
}
