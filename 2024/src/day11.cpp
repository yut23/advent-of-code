/******************************************************************************
 * File:        day11.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-11
 *****************************************************************************/

#include "day11.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    auto stones = aoc::day11::read_input(infile);
    if constexpr (aoc::DEBUG) {
        std::cerr << "Initial arrangement:\n";
        for (const auto &stone : stones) {
            std::cerr << stone << " ";
        }
        std::cerr << "\n\n";
    }

    for (int i = 1; i <= 25; ++i) {
        aoc::day11::blink(stones);
        if constexpr (aoc::DEBUG) {
            std::cerr << "After " << i << " blinks: (" << stones.size()
                      << " stones)\n";
            for (const auto &stone : stones) {
                std::cerr << stone << " ";
            }
            std::cerr << "\n\n";
        }
    }
    std::cout << stones.size() << "\n";

    return 0;
}
