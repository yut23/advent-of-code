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
    auto args = aoc::parse_args(argc, argv);
    const bool is_example = args.input_type == aoc::InputType::EXAMPLE;
    aoc::Pos bounds = is_example ? aoc::Pos{11, 7} : aoc::Pos{101, 103};

    auto robots = aoc::day14::Robots::read(args.infile, bounds);

    for (int time = 0; time < 10000; ++time) {
        if (time == 100) {
            // part 1
            std::cout << robots.safety_factor() << "\n";
            if (is_example) {
                break;
            }
        }

        if (robots.largest_clump() > 10) {
            // part 2
            std::cout << time << "\n";
            break;
        }

        robots.update();
    }

    return 0;
}
