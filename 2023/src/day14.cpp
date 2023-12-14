/******************************************************************************
 * File:        day14.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-14
 *****************************************************************************/

#include "day14.hpp"
#include "lib.hpp"  // for parse_args, Pos, Direction, DEBUG
#include <array>    // for array
#include <iostream> // for cout, cerr
#include <map>      // for map
#include <set>      // for set

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    std::map<std::set<aoc::Pos>, long> states;
    auto platform = aoc::day14::read_platform(infile);
    states[platform.round_rock_lookup] = 0;

    if constexpr (aoc::DEBUG) {
        std::cerr << "before tilting:\n";
        std::cerr << platform << "\n";
    }

    constexpr long max_step = 4'000'000'000;
    constexpr std::array<aoc::Direction, 4> directions{
        aoc::Direction::up,
        aoc::Direction::left,
        aoc::Direction::down,
        aoc::Direction::right,
    };

    long cycle_length = 0;
    for (long step = 0; step < max_step; ++step) {
        platform.tilt(directions[step % 4]);
        if (cycle_length == 0) {
            auto it = states.find(platform.round_rock_lookup);
            if (it != states.end()) {
                cycle_length = step - it->second;
                // skip to the final cycle iteration
                while (step + cycle_length < max_step) {
                    step += cycle_length;
                }
            } else {
                states[platform.round_rock_lookup] = step;
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
