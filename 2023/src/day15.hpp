/******************************************************************************
 * File:        day15.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-15
 *****************************************************************************/

#ifndef DAY_HPP_SD0ITZCR
#define DAY_HPP_SD0ITZCR

#include <iostream> // for istream
#include <string>   // for string, getline
#include <vector>   // for vector

namespace aoc::day15 {

int hash_algorithm(const std::string &s) {
    int value = 0;
    for (char c : s) {
        value += c;
        value *= 17;
        value %= 256;
    }
    return value;
}

std::vector<std::string> read_steps(std::istream &is) {
    std::vector<std::string> steps;
    std::string tmp;
    while (std::getline(is, tmp, ',')) {
        // remove trailing newline(s)
        while (tmp.ends_with('\n')) {
            tmp.pop_back();
        }
        steps.push_back(tmp);
    }
    return steps;
}

} // namespace aoc::day15

#endif /* end of include guard: DAY_HPP_SD0ITZCR */
