/******************************************************************************
 * File:        day25.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-01-27
 *****************************************************************************/

#include "day25.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv).infile;

    auto [keys, locks] = aoc::day25::read_input(infile);

    int count = 0;
    for (const auto &lock : locks) {
        if constexpr (aoc::DEBUG) {
            std::cerr << "checking " << lock << ":\n";
        }
        for (const auto &key : keys) {
            if constexpr (aoc::DEBUG) {
                std::cerr << "  " << key << "";
            }
            if (lock.check_fit(key)) {
                if constexpr (aoc::DEBUG) {
                    std::cerr << ": fits";
                }
                ++count;
            }
            if constexpr (aoc::DEBUG) {
                std::cerr << "\n";
            }
        }
    }
    std::cout << count << "\n";

    return 0;
}
