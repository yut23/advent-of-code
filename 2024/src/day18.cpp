/******************************************************************************
 * File:        day18.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-18
 *****************************************************************************/

#include "day18.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout

int main(int argc, char **argv) {
    auto args = aoc::parse_args(argc, argv);
    unsigned int size, byte_count;
    if (args.input_type == aoc::InputType::EXAMPLE) {
        size = 7;
        byte_count = 12;
    } else {
        size = 71;
        byte_count = 1024;
    }

    aoc::ds::Grid<bool> grid(size, size, true);
    auto corrupted_bytes = aoc::day18::read_input(args.infile);
    for (unsigned int i = 0; i < byte_count; ++i) {
        grid[corrupted_bytes[i]] = false;
    }

    if constexpr (aoc::DEBUG) {
        grid.custom_print(std::cerr, [](auto &os, auto &, bool passable) {
            os << (passable ? '.' : '#');
        });
    }

    std::cout << aoc::day18::shortest_path_distance(grid) << "\n";

    return 0;
}
