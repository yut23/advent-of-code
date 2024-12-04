/******************************************************************************
 * File:        day02.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-02
 *****************************************************************************/

#ifndef DAY02_HPP_QMMUZCBK
#define DAY02_HPP_QMMUZCBK

#include <algorithm> // for copy
#include <cstdlib>   // for abs, size_t
#include <iostream>  // for istream
#include <sstream>   // for istringstream
#include <string>    // for string, getline
#include <utility>   // for move
#include <vector>    // for vector

namespace aoc::day02 {

bool is_safe(const std::vector<int> &levels) {
    bool increasing = levels.at(1) > levels[0];
    for (std::size_t i = 0; i < levels.size() - 1; ++i) {
        int diff = levels[i + 1] - levels[i];
        if (diff == 0) {
            return false;
        }
        if (increasing != (diff > 0)) {
            return false;
        }
        if (std::abs(diff) > 3) {
            return false;
        }
    }
    return true;
}

bool is_safe_with_dampener(const std::vector<int> &levels) {
    // not elegant, but it works...
    std::vector<int> adjusted(levels.size() - 1);
    std::copy(levels.cbegin() + 1, levels.cend(), adjusted.begin());

    for (std::size_t i = 0; i < levels.size(); ++i) {
        if (is_safe(adjusted)) {
            return true;
        }
        if (i < adjusted.size()) {
            adjusted[i] = levels[i];
        }
    }
    return false;
}

auto read_input(std::istream &is) {
    // read file line-by-line
    std::string line;
    std::vector<std::vector<int>> reports;
    while (std::getline(is, line)) {
        std::istringstream ss(line);
        std::vector<int> levels;
        int tmp;
        while (ss >> tmp) {
            levels.push_back(tmp);
        }
        reports.push_back(std::move(levels));
    }
    return reports;
}

} // namespace aoc::day02

#endif /* end of include guard: DAY02_HPP_QMMUZCBK */
