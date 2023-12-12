/******************************************************************************
 * File:        day11.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-11
 *****************************************************************************/

#include "day11.hpp"
#include "lib.hpp"  // for parse_args, Pos, DEBUG
#include <cstddef>  // for size_t
#include <iostream> // for cout, cerr

int part_1(std::vector<aoc::Pos> galaxies) {
    if constexpr (aoc::DEBUG) {
        std::cerr << "before expanding:\n";
        for (const auto &pos : galaxies) {
            std::cerr << "  " << pos << "\n";
        }
    }
    aoc::day11::expand(galaxies);
    if constexpr (aoc::DEBUG) {
        std::cerr << "\nafter expanding:\n";
        for (const auto &pos : galaxies) {
            std::cerr << "  " << pos << "\n";
        }
    }
    int path_sum = 0;
    for (std::size_t i = 0; i < galaxies.size(); ++i) {
        for (std::size_t j = i + 1; j < galaxies.size(); ++j) {
            path_sum += (galaxies[i] - galaxies[j]).manhattan_distance();
        }
    }
    return path_sum;
}

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    std::vector<aoc::Pos> galaxies = aoc::day11::read_image(infile);

    std::cout << part_1(galaxies) << "\n";

    return 0;
}
