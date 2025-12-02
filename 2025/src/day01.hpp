/******************************************************************************
 * File:        day01.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-12-01
 *****************************************************************************/

#ifndef DAY01_HPP_XFIW6CVE
#define DAY01_HPP_XFIW6CVE

#include <iostream> // for istream
#include <string>   // for string, getline
#include <vector>   // for vector

namespace aoc::day01 {

auto read_input(std::istream &is) {
    // read file line-by-line
    std::string line;
    std::vector<int> rotations;
    while (std::getline(is, line)) {
        int rotation = std::atoi(line.c_str() + 1);
        if (line[0] == 'L') {
            rotation *= -1;
        }
        rotations.push_back(rotation);
    }
    return rotations;
}

} // namespace aoc::day01

#endif /* end of include guard: DAY01_HPP_XFIW6CVE */
