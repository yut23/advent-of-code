/******************************************************************************
 * File:        day13.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-13
 *****************************************************************************/

#include "day13.hpp"
#include "lib.hpp"  // for parse_args, DEBUG
#include <fstream>  // for ifstream
#include <iostream> // for cout, cerr

void print_grid(std::ostream &os, const aoc::day13::Grid &grid) {
    for (const auto &line : grid) {
        os << line << "\n";
    }
}

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    auto grids = aoc::day13::read_grids(infile);

    int part_1 = 0;
    int part_2 = 0;
    for (const auto &grid : grids) {
        using namespace aoc::day13;
        part_1 += 100 * find_reflection(grid);
        part_2 += 100 * find_reflection(grid, 1);

        auto transposed_grid = transpose(grid);
        part_1 += find_reflection(transposed_grid);
        part_2 += find_reflection(transposed_grid, 1);

        if constexpr (aoc::DEBUG) {
            print_grid(std::cerr, grid);
            std::cerr << "\n";
            print_grid(std::cerr, transposed_grid);
            std::cerr << "\n\n";
        }
    }

    std::cout << part_1 << "\n";
    std::cout << part_2 << "\n";

    return 0;
}
