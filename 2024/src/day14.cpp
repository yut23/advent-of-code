/******************************************************************************
 * File:        day14.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-14
 *****************************************************************************/

#include "day14.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);
    const bool is_example =
        std::string{argv[1]}.find("example") != std::string::npos;
    aoc::Pos bounds = is_example ? aoc::Pos{11, 7} : aoc::Pos{101, 103};

    auto robots = aoc::day14::read_input(infile);

    for (int time = 0; time < 100; ++time) {
        for (auto &robot : robots) {
            robot.update(bounds);
        }
    }

    std::cout << aoc::day14::safety_factor(robots, bounds) << "\n";

    return 0;
}
