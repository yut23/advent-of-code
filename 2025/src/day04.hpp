/******************************************************************************
 * File:        day04.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-12-04
 *****************************************************************************/

#ifndef DAY04_HPP_3PKMNXVL
#define DAY04_HPP_3PKMNXVL

#include "ds/grid.hpp" // for Grid
#include "lib.hpp"     // for Delta, Pos
#include <iostream>    // for istream
#include <string>      // for string, getline

namespace aoc::day04 {

using aoc::ds::Grid;

int count_forklift_accessible(const Grid<char> &grid) {
    int count = 0;
    grid.for_each([&count, &grid](char c, const Pos &center) {
        if (c != '@') {
            return;
        }
        int adj_count = 0;
        grid.chebyshev_kernel(center, [&adj_count](char adj) {
            if (adj == '@') {
                ++adj_count;
            }
        });
        // the 4 in the question doesn't include the center roll
        if (adj_count < 5) {
            ++count;
        }
    });
    return count;
}

Grid<char> read_input(std::istream &is) {
    return Grid<char>{aoc::read_lines(is)};
}

} // namespace aoc::day04

#endif /* end of include guard: DAY04_HPP_3PKMNXVL */
