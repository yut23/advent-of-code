/******************************************************************************
 * File:        day09.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-09
 *****************************************************************************/

#include "day09.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout

int main(int argc, char **argv) {
    using namespace aoc::day09;
    std::ifstream infile = aoc::parse_args(argc, argv);

    DiskLayout disk_layout;
    infile >> disk_layout;
    if constexpr (aoc::DEBUG) {
        std::cerr << "initial:   " << disk_layout << "\n";
    }

    disk_layout.compact();
    if constexpr (aoc::DEBUG) {
        std::cerr << "compacted: " << disk_layout << "\n";
    }
    std::cout << disk_layout.calculate_checksum() << "\n";

    return 0;
}
