/******************************************************************************
 * File:        day07.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-12-07
 *****************************************************************************/

#ifndef DAY07_HPP_XQTFHGV1
#define DAY07_HPP_XQTFHGV1

#include "ds/grid.hpp" // for Grid
#include "lib.hpp"     // for read_whole_stream
#include <cassert>     // for assert
#include <iostream>    // for ostream, istream
#include <string>      // for string
#include <utility>     // for swap
#include <vector>      // for vector

namespace aoc::day07 {

using aoc::ds::Grid;

enum class TileType : char {
    EMPTY = '.',
    SPLITTER = '^',
    BEAM = '|',
};
std::ostream &operator<<(std::ostream &os, TileType tile) {
    os << static_cast<char>(tile);
    return os;
}

using ManifoldGrid = aoc::ds::Grid<TileType>;

ManifoldGrid read_input(std::istream &is) {
    std::string data = aoc::read_whole_stream(is);
    int width = data.find_first_of('\n');
    assert(data.length() % (width + 1) == 0);
    int height = data.length() / (width + 1);
    std::vector<TileType> transformed{};
    transformed.reserve(height * width);
    for (char c : data) {
        if (c == 'S') {
            transformed.push_back(TileType::BEAM);
        } else if (c != '\n') {
            transformed.push_back(TileType(c));
        }
    }
    return ManifoldGrid(width, height, transformed);
}

int propagate_tachyon_beams(ManifoldGrid &grid) {
    ManifoldGrid::iterator prev_row_it = grid.begin();
    ManifoldGrid::iterator curr_row_it = prev_row_it + 1;
    const ManifoldGrid::iterator row_end = grid.end();
    int split_count = 0;
    while (curr_row_it != row_end) {
        auto prev_row = *prev_row_it, curr_row = *curr_row_it;
        for (int i = 0; i < grid.width; ++i) {
            using enum TileType;
            TileType &curr = curr_row[i];
            TileType &prev = prev_row[i];
            switch (curr) {
            case EMPTY:
                if (prev == BEAM) {
                    curr = BEAM;
                }
                break;
            case BEAM:
                // set by an adjacent splitter
                break;
            case SPLITTER:
                if (prev == BEAM) {
                    assert(i > 0);
                    assert(i < grid.width - 1);
                    curr_row[i - 1] = BEAM;
                    curr_row[i + 1] = BEAM;
                    ++split_count;
                }
                break;
            }
        }

        // leapfrog the two iterators
        prev_row_it += 2;
        std::swap(curr_row_it, prev_row_it);
    }
    return split_count;
}

} // namespace aoc::day07

#endif /* end of include guard: DAY07_HPP_XQTFHGV1 */
