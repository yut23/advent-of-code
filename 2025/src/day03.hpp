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

int max_joltage(const std::string &bank) {
    // find maximum value excluding last battery
    auto first = std::max_element(bank.begin(), bank.end() - 1);
    // find maximum value after first value
    auto second = std::max_element(first + 1, bank.end());
    return (*first - '0') * 10 + (*second - '0');
}

} // namespace aoc::day03

#endif /* end of include guard: DAY03_HPP_0X4VI3US */
