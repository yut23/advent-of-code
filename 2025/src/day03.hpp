/******************************************************************************
 * File:        day03.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-12-03
 *****************************************************************************/

#ifndef DAY03_HPP_0X4VI3US
#define DAY03_HPP_0X4VI3US

#include <algorithm> // for max_element
#include <iostream>  // for istream
#include <string>    // for string, getline
#include <utility>   // for move
#include <vector>    // for vector

namespace aoc::day03 {

auto read_input(std::istream &is) {
    // read file line-by-line
    std::vector<std::string> banks;
    std::string line;
    while (std::getline(is, line)) {
        banks.emplace_back(std::move(line));
    }
    return banks;
}

long max_joltage(const std::string &bank, int count) {
    long value = 0;
    std::string::const_iterator start = bank.begin();
    for (int i = 1; i <= count; ++i) {
        // find maximum value, excluding the last (count - i) batteries
        start = std::max_element(start, bank.end() - (count - i));
        value *= 10;
        value += *start - '0';
        ++start;
    }
    return value;
}

} // namespace aoc::day03

#endif /* end of include guard: DAY03_HPP_0X4VI3US */
