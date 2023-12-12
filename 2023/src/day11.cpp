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

long sum_paths(const std::vector<aoc::Pos> &galaxies) {
    long path_sum = 0;
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

    std::cout << sum_paths(aoc::day11::expand(galaxies, 2)) << "\n";
    if constexpr (aoc::DEBUG) {
        std::cout << sum_paths(aoc::day11::expand(galaxies, 10)) << "\n";
        std::cout << sum_paths(aoc::day11::expand(galaxies, 100)) << "\n";
    }
    std::cout << sum_paths(aoc::day11::expand(galaxies, 1000000)) << "\n";

    return 0;
}
