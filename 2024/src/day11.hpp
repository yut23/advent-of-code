/******************************************************************************
 * File:        day11.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-11
 *****************************************************************************/

#ifndef DAY11_HPP_UOIOTEZD
#define DAY11_HPP_UOIOTEZD

#include <iostream> // for istream
#include <list>     // for list
#include <string>   // for string, to_string, stoi

namespace aoc::day11 {

using Stone = long;

std::list<Stone> read_input(std::istream &is) {
    std::list<Stone> stones;
    Stone stone;
    while (is >> stone) {
        stones.push_back(stone);
    }
    return stones;
}

void blink(std::list<Stone> &stones) {
    for (auto it = stones.begin(); it != stones.end(); ++it) {
        if (*it == 0) {
            *it = 1;
        } else if (std::string digits = std::to_string(*it);
                   digits.size() % 2 == 0) {
            int half_size = digits.size() / 2;
            stones.insert(it, std::stoi(digits.substr(0, half_size)));
            *it = std::stoi(digits.substr(half_size));
        } else {
            *it *= 2024;
        }
    }
}

} // namespace aoc::day11

#endif /* end of include guard: DAY11_HPP_UOIOTEZD */
