/******************************************************************************
 * File:        day07.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-07
 *****************************************************************************/

#include "day07.hpp"
#include "lib.hpp"
#include <algorithm> // for sort
#include <cstddef>   // for size_t
#include <iostream>  // for cout
#include <vector>    // for vector

int part_1(const std::vector<aoc::day07::Hand> &hands) {
    int winnings = 0;
    for (std::size_t i = 0; i < hands.size(); ++i) {
        winnings += (i + 1) * hands[i].bid;
    }
    return winnings;
}

int part_2(std::vector<aoc::day07::Hand> hands) {
    for (auto &hand : hands) {
        hand.jacks_to_jokers();
    }
    // re-sort in ascending order
    std::ranges::sort(hands);
    return part_1(hands);
}

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    using namespace aoc::day07;
    auto hands = read_hands(infile);
    // sort in ascending order
    std::ranges::sort(hands);

    std::cout << part_1(hands) << "\n";
    std::cout << part_2(hands) << "\n";
    return 0;
}
