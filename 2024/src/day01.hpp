/******************************************************************************
 * File:        day01.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-01
 *****************************************************************************/

#ifndef DAY01_HPP_PV7TQXOA
#define DAY01_HPP_PV7TQXOA

#include <iostream> // for istream
#include <utility>  // for pair
#include <vector>   // for vector

namespace aoc::day01 {

std::pair<std::vector<int>, std::vector<int>> read_input(std::istream &is) {
    // read file line-by-line
    std::vector<int> left, right;
    int l, r;
    while (is >> l >> r) {
        left.push_back(l);
        right.push_back(r);
    }
    return {left, right};
}

} // namespace aoc::day01

#endif /* end of include guard: DAY01_HPP_PV7TQXOA */
