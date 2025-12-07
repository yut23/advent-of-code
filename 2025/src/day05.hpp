/******************************************************************************
 * File:        day05.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-12-05
 *****************************************************************************/

#ifndef DAY05_HPP_WG3PCRXA
#define DAY05_HPP_WG3PCRXA

#include "ds/range_lookup.hpp"
#include "lib.hpp"  // for read_vector
#include <iostream> // for istream
#include <sstream>  // for istringstream
#include <string>   // for string, getline
#include <utility>  // for pair
#include <vector>   // for vector

namespace aoc::day05 {

std::pair<ds::RangeLookup, std::vector<long>> read_input(std::istream &is) {
    // read file line-by-line
    std::string line;
    ds::RangeLookup lookup{};
    while (std::getline(is, line)) {
        if (line.empty()) {
            break;
        }
        std::istringstream iss(line);
        long start = 0, end = 0;
        iss >> start >> aoc::expect_input('-') >> end;
        if (iss) {
            lookup.add_range(start, end + 1);
        }
    }
    return {lookup, aoc::read_vector<long>(is)};
}

} // namespace aoc::day05

#endif /* end of include guard: DAY05_HPP_WG3PCRXA */
