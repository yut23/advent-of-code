/******************************************************************************
 * File:        day24.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-25
 *****************************************************************************/

#include "day24.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv).infile;

    auto sim = aoc::day24::LogicSim::read(infile);
    sim.evaluate();

    std::cout << sim.z_value() << "\n";

    return 0;
}
