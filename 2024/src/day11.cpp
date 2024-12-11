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

    auto stones = aoc::day11::Stones::read(infile);
    if constexpr (aoc::DEBUG) {
        std::cerr << "Initial arrangement:\n";
        std::cerr << stones << "\n\n";
    }

    // part 1
    int i;
    for (i = 1; i <= 25; ++i) {
        stones.blink();
        if constexpr (aoc::DEBUG) {
            if (i <= 6) {
                std::cerr << "After " << i << " blinks: (" << stones.count()
                          << " stones)\n";
                std::cerr << stones << "\n\n";
            } else {
                std::cerr << i << ": " << stones.count() << "\n";
            }
        }
    }
    std::cout << stones.count() << "\n";

    // part 2
    for (; i <= 75; ++i) {
        stones.blink();
        if constexpr (aoc::DEBUG) {
            std::cerr << i << ": " << stones.count() << "\n";
        }
    }
    std::cout << stones.count() << "\n";

    return 0;
}
