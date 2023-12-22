/******************************************************************************
 * File:        day14.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-14
 *****************************************************************************/

#include "day14.hpp"
#include "lib.hpp"   // for parse_args, Pos, AbsDirection, DEBUG
#include <algorithm> // for sort
#include <array>     // for array
#include <iostream>  // for cout, cerr
#include <map>       // for map
// IWYU pragma: no_include <functional>  // for identity, less (ranges::sort)
// IWYU pragma: no_include <utility>  // for pair (map)

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    std::map<std::vector<aoc::Pos>, long> states;
    auto platform = aoc::day14::read_platform(infile);
    std::ranges::sort(platform.round_rocks);

    if constexpr (aoc::DEBUG) {
        std::cerr << "before tilting:\n";
        std::cerr << platform << "\n";
    }

    constexpr long max_step = 4'000'000'000;
    constexpr std::array<aoc::AbsDirection, 4> directions{
        aoc::AbsDirection::north,
        aoc::AbsDirection::west,
        aoc::AbsDirection::south,
        aoc::AbsDirection::east,
    };

    long cycle_length = 0;
    for (long step = 0; step < max_step; ++step) {
        platform.tilt(directions[step % 4]);
        if (cycle_length == 0) {
            std::ranges::sort(platform.round_rocks);
            auto it = states.find(platform.round_rocks);
            if (it != states.end()) {
                cycle_length = step - it->second;
                // skip to the final cycle iteration
                while (step + cycle_length < max_step) {
                    step += cycle_length;
                }
            } else {
                states.try_emplace(platform.round_rocks, step);
            }
        }
        if (step == 0) {
            if constexpr (aoc::DEBUG) {
                std::cerr << "after tilting:\n";
                std::cerr << platform << "\n";
            }
            // part 1
            std::cout << platform.calculate_load() << "\n";
        }
    }

    // part 2
    std::cout << platform.calculate_load() << "\n";

    return 0;
}
