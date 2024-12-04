/******************************************************************************
 * File:        day23.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-24
 *****************************************************************************/

#include "day23.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout

void solve(int argc, char **argv, bool print) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    auto trail_map = aoc::day23::TrailMap::read(infile);

    int part_1 = trail_map.part_1();
    if (print) {
        std::cout << part_1 << "\n";
    }

    int part_2 = trail_map.part_2();
    if (print) {
        std::cout << part_2 << "\n";
    }
}

int main(int argc, char **argv) {
    // constexpr int N = aoc::FAST ? 1000 : 1;
    constexpr int N = 1;
    for (int i = 0; i < N; ++i) {
        solve(argc, argv, i == 0);
    }
    return 0;
}
