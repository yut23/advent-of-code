/******************************************************************************
 * File:        day08.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-08
 *****************************************************************************/

#include "day08.hpp"
#include "lib.hpp"  // for parse_args, DEBUG
#include <cassert>  // for assert
#include <iostream> // for cout
#include <numeric>  // for lcm
#include <string>   // for string

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

long part_2(const std::string &directions, const aoc::day08::Network &network) {
    auto cycles = aoc::day08::find_cycles(directions, network);
    long multiple = 1;
    // TODO: this works for the real input, but not for the examples.
    for (const auto &cycle : cycles) {
        if constexpr (aoc::DEBUG) {
            std::cerr << cycle << "\n";
        }
        assert(cycle.start == cycle.length);
        multiple = std::lcm(multiple, cycle.length);
    }
    return multiple;
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
