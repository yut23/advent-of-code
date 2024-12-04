/******************************************************************************
 * File:        day14.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-14
 *****************************************************************************/

#include "day14.hpp"
#include "lib.hpp"       // for parse_args, DEBUG
#include <fstream>       // for ifstream
#include <iostream>      // for cout, cerr
#include <unordered_map> // for unordered_map
// IWYU pragma: no_include <functional>  // for hash (unordered_map)

void solve(int argc, char **argv, bool print) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    std::unordered_map<std::vector<bool>, long> states;
    auto platform = aoc::day14::read_platform(infile);

    if constexpr (aoc::DEBUG) {
        std::cerr << "before tilting:\n" << platform << "\n";
    }
    // rotate so north is the initial tilt direction
    platform.rotate_ccw();
    if constexpr (aoc::DEBUG) {
        std::cerr << "after rotating counter-clockwise:\n" << platform << "\n";
    }

    constexpr long max_step = 4'000'000'000;

    long cycle_length = 0;
    for (long step = 0; step < max_step; ++step) {
        platform.tilt();
        if (cycle_length == 0 && step % 4 == 0) {
            const std::vector<bool> round_rocks = platform.round_rocks();
            auto it = states.find(round_rocks);
            if (it != states.end()) {
                cycle_length = step - it->second;
                if constexpr (aoc::DEBUG) {
                    std::cerr << "found cycle: " << it->second << " -> " << step
                              << "\n";
                }
                // skip to the final cycle iteration
                step += (max_step - step - 1) / cycle_length * cycle_length;
                if constexpr (aoc::DEBUG) {
                    std::cerr << "skipped to " << step << "\n";
                }
            } else {
                states.try_emplace(round_rocks, step);
            }
        }
        if (step == 0) {
            if constexpr (aoc::DEBUG) {
                std::cerr << "after tilting:\n" << platform << "\n";
            }
            // part 1
            int part_1 = platform.calculate_load();
            if (print) {
                std::cout << part_1 << "\n";
            }
        }
        platform.rotate_cw();
    }

    // part 2
    int part_2 = platform.calculate_load();
    if (print) {
        std::cout << part_2 << "\n";
    }
}

int main(int argc, char **argv) {
    constexpr int N = 1;
    // constexpr int N = 100;
    for (int i = 0; i < N; ++i) {
        solve(argc, argv, i == 0);
    }
    return 0;
}
