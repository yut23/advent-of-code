/******************************************************************************
 * File:        day02.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-02
 *****************************************************************************/

#include "day02.hpp"
#include "lib.hpp"
#include <algorithm> // for ranges::count_if
#include <iostream>  // for cout

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    auto reports = aoc::day02::read_input(infile);

    int count = std::ranges::count_if(reports, aoc::day02::is_safe);
    std::cout << count << "\n";

    return 0;
}
