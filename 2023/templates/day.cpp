/******************************************************************************
 * File:        day{{DAY}}.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     {{DATE}}
 *****************************************************************************/

#include "day{{DAY}}.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv).infile;

    auto data = aoc::day{{DAY}}::read_input(infile);

    return 0;
}
