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
    auto it = corrupted_bytes.begin();
    for (; it != corrupted_bytes.begin() + byte_count; ++it) {
        grid[*it] = false;
    }

    if constexpr (aoc::DEBUG) {
        grid.custom_print(std::cerr, [](bool passable) {
            std::cerr << (passable ? '.' : '#');
        });
    }

    std::cout << aoc::day18::shortest_path(grid).size() - 1 << "\n";

    aoc::Pos cutoff_pos =
        aoc::day18::find_cutoff_pos(grid, it, corrupted_bytes.end());
    std::cout << cutoff_pos.x << ',' << cutoff_pos.y << "\n";

    return 0;
}
