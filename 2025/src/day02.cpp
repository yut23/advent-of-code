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

    std::string line;
    long part1 = 0;
    while (std::getline(infile, line, ',')) {
        part1 += aoc::day02::sum_invalid_ids(line);
    }
    std::cout << part1 << "\n";

    return 0;
}
