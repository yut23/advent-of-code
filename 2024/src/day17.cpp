/******************************************************************************
 * File:        day17.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-17
 *****************************************************************************/

#include "day17.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv).infile;

    auto computer = aoc::day17::ChronoComputer::read(infile);

    auto output = computer.execute();
    for (auto it = output.begin(); it != output.end(); ++it) {
        if (it != output.begin()) {
            std::cout << ',';
        }
        std::cout << aoc::as_number(*it);
    }
    std::cout << "\n";

    return 0;
}
