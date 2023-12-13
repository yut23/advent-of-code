/******************************************************************************
 * File:        day13.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-13
 *****************************************************************************/

#include "lib.hpp"
#include <cassert>  // for assert
#include <cstddef>  // for size_t
#include <iostream> // for istream
#include <string>   // for string, getline
#include <vector>   // for vector

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

/// returns 0 if no reflection is found
std::size_t find_reflection(const Grid &grid) {
    // just check every row to see if it's a line of reflection
    // if this is a bottleneck, I'll improve it later
    for (std::size_t i = 1; i < grid.size(); ++i) {
        bool is_reflection = true;
        for (int j = 0; i - j > 0 && i + j < grid.size(); ++j) {
            if (grid[i - j - 1] != grid[i + j]) {
                is_reflection = false;
                break;
            }
        }
        if (is_reflection) {
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
