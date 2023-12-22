/******************************************************************************
 * File:        day13.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-13
 *****************************************************************************/

#ifndef DAY13_HPP_W3VGLUGR
#define DAY13_HPP_W3VGLUGR

#include <algorithm> // for mismatch
#include <cstddef>   // for size_t
#include <iostream>  // for istream
#include <string>    // for string, getline
#include <utility>   // for move
#include <vector>    // for vector
// IWYU pragma: no_include <functional>  // for identity, equal_to (ranges::mismatch)

namespace aoc::day13 {

using Grid = std::vector<std::string>;

Grid transpose(const Grid &grid) {
    if (grid.size() == 0) {
        return {};
    }
    Grid new_grid(grid[0].size());
    for (const std::string &row : grid) {
        for (std::size_t i = 0; i < row.size(); ++i) {
            new_grid[i].push_back(row[i]);
        }
    }
    return new_grid;
}

/// returns 0, 1, or 2 (for more than 1 mismatch)
int count_mismatches(const std::string &s1, const std::string &s2) {
    const auto result =
        std::ranges::mismatch(s1.begin(), s1.end(), s2.begin(), s2.end());
    if (result.in1 == s1.end()) {
        return 0;
    }
    if (std::mismatch(result.in1 + 1, s1.end(), result.in2 + 1, s2.end())
            .first == s1.end()) {
        return 1;
    }
    return 2;
}

/// returns 0 if no reflection is found
std::size_t find_reflection(const Grid &grid, const int target_mismatches = 0) {
    // just check every row to see if it's a line of reflection
    for (std::size_t i = 1; i < grid.size(); ++i) {
        int mismatches = 0;
        for (int j = 0; i - j > 0 && i + j < grid.size(); ++j) {
            if (grid[i - j - 1] != grid[i + j]) {
                mismatches += count_mismatches(grid[i - j - 1], grid[i + j]);
                if (mismatches > target_mismatches) {
                    break;
                }
            }
        }
        if (mismatches == target_mismatches) {
            return i;
        }
    }
    return 0;
}

std::vector<Grid> read_grids(std::istream &is) {
    std::vector<Grid> grids(1);
    std::string line;
    Grid curr_grid;
    while (std::getline(is, line)) {
        if (line.empty()) {
            grids.emplace_back();
            continue;
        } else {
            grids.back().push_back(std::move(line));
        }
    }
    return grids;
}

} // namespace aoc::day13

#endif /* end of include guard: DAY13_HPP_W3VGLUGR */
