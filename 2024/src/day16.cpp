/******************************************************************************
 * File:        day16.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-16
 *****************************************************************************/

#include "day16.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout

int main(int argc, char **argv) {
    auto args = aoc::parse_args(argc, argv);

    auto maze = aoc::day16::Maze::read(args.infile);

    int distance = maze.find_shortest_path();
    std::cout << distance << "\n";
    if (args.input_type == aoc::InputType::MAIN) {
        assert(distance < 76404);
    }

    return 0;
}
