/******************************************************************************
 * File:        day02.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-12-02
 *****************************************************************************/

#include "day02.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv).infile;

    using namespace aoc::day02;
    Range range{};
    long part1 = 0;
    while (infile >> range) {
        if constexpr (aoc::DEBUG)
            std::cerr << "range " << range << ":";
        part1 += sum_invalid_ids(range);
    }
    std::cout << part1 << "\n";

    return 0;
}
