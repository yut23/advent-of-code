/******************************************************************************
 * File:        day04.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-04
 *****************************************************************************/

#include "day04.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    auto grid = aoc::day04::read_input(infile);

    // part 1
    int count = 0;
    aoc::Pos pos(0, 0);
    for (pos.y = 0; pos.y < grid.height; ++pos.y) {
        for (pos.x = 0; pos.x < grid.width; ++pos.x) {
            count += aoc::day04::count_xmas_at(grid, pos);
        }
    }
    std::cout << count << "\n";

    return 0;
}
