/******************************************************************************
 * File:        day04.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-04
 *****************************************************************************/

#ifndef DAY04_HPP_MZNV9FEE
#define DAY04_HPP_MZNV9FEE

#include "ds/grid.hpp"      // for Grid
#include "lib.hpp"          // for Delta, Pos
#include <array>            // for array
#include <cstddef>          // for size_t
#include <initializer_list> // for initializer_list
#include <iostream>         // for istream
#include <string>           // for string, getline
#include <utility>          // for move
#include <vector>           // for vector

namespace aoc::day04 {

using aoc::ds::Grid, aoc::Pos, aoc::Delta;

constexpr std::initializer_list<Delta> ADJ_DELTAS = {
    // clang-format off
    {-1, -1}, {-1, 0}, {-1, 1},
    { 0, -1},          { 0, 1},
    { 1, -1}, { 1, 0}, { 1, 1},
    // clang-format on
};
constexpr std::array<char, 4> WORD = {'X', 'M', 'A', 'S'};

int count_xmas_at(const Grid<char> &grid, const Pos &p) {
    int count = 0;
    // exit early if the first letter doesn't match
    if (grid[p] != WORD[0]) {
        return count;
    }
    for (const auto d : ADJ_DELTAS) {
        bool valid = true;
        Pos q = p + d;
        for (std::size_t i = 1; i < WORD.size(); ++i, q += d) {
            if (!grid.in_bounds(q) || grid[q] != WORD[i]) {
                valid = false;
                break;
            }
        }
        if (valid) {
            ++count;
        }
    }
    return count;
}

Grid<char> read_input(std::istream &is) {
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(is, line)) {
        lines.push_back(std::move(line));
    }
    return Grid<char>{std::move(lines)};
}

} // namespace aoc::day04

#endif /* end of include guard: DAY04_HPP_MZNV9FEE */
