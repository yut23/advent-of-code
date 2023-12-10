/******************************************************************************
 * File:        day08.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-08
 *****************************************************************************/

#include "day08.hpp"
#include "lib.hpp"   // for parse_args, DEBUG
#include <algorithm> // for max, any_of
#include <cstddef>   // for size_t
#include <iostream>  // for cout, cerr
#include <numeric>   // for lcm
#include <string>    // for string

int part_1(const std::string &directions, const aoc::day08::Network &network) {
    int steps = 0;
    const aoc::day08::Node *curr = network.get("AAA");
    auto dir_it = directions.begin();
    while (curr->id != "ZZZ") {
        curr = network.follow(curr, *dir_it);
        // repeat the direction string if we hit the end
        if (++dir_it == directions.end()) {
            dir_it = directions.begin();
        }
        ++steps;
    }
    return steps;
}

/// fast version that works for the real input, using LCM
long part_2_fast(const std::vector<aoc::day08::CycleInfo> &cycles) {
    long multiple = 1;
    for (const auto &cycle : cycles) {
        multiple = std::lcm(multiple, cycle.length);
    }
    return multiple;
}

/// slower version that works for any input, using accelerated time-stepping
long part_2_slow(const std::vector<aoc::day08::CycleInfo> &cycles) {
    using namespace aoc::day08;
    // follow the cycle with the longest average time between encountering
    // finish nodes
    const CycleInfo &base_cycle =
        std::ranges::max(cycles, {}, [](const CycleInfo &cycle) {
            return cycle.length / cycle.entries.size();
        });
    std::vector<long> diffs;
    if (base_cycle.entries.size() > 1) {
        for (std::size_t i = 0; i < base_cycle.entries.size(); ++i) {
            diffs.push_back(
                base_cycle.entries[(i + 1) % base_cycle.entries.size()].step -
                base_cycle.entries[i].step);
        }
    } else {
        diffs.push_back(base_cycle.length);
    }
    long step = 0;
    std::size_t i = 0;
    long counter = 0;
    // while any of the cycles aren't at a finish node:
    while (std::ranges::any_of(cycles, [&step](const CycleInfo &cycle) {
        return !cycle.at_finish(step);
    })) {
        step += diffs[i];
        i = (i + 1) % diffs.size();
        if constexpr (aoc::DEBUG) {
            if (counter % 1'000'000 == 0) {
                std::cerr << ".";
                std::cerr.flush();
            }
            ++counter;
        }
    }
    return step;
}

long part_2(const std::string &directions, const aoc::day08::Network &network) {
    auto cycles = aoc::day08::find_cycles(directions, network);
    bool fast_valid = true;
    for (const auto &cycle : cycles) {
        if constexpr (aoc::DEBUG) {
            std::cerr << cycle << "\n";
        }
        fast_valid &= cycle.start == cycle.length && cycle.entries.size() == 1;
    }
    if (fast_valid) {
        return part_2_fast(cycles);
    } else {
        return part_2_slow(cycles);
    }
}

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    std::string directions;
    infile >> directions;
    auto network = aoc::day08::read_maps(infile);

    std::cout << part_1(directions, network) << "\n";
    std::cout << part_2(directions, network) << "\n";

    return 0;
}
