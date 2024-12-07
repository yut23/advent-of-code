/******************************************************************************
 * File:        day06.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-06
 *****************************************************************************/

#include "day06.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    aoc::day06::GuardSim guard_sim = aoc::day06::read_input(infile);

    guard_sim.run();
    // part 1
    std::cout << guard_sim.count_visited() << "\n";
    // part 2
    std::cout << guard_sim.count_new_loops() << "\n";

    return 0;
}
