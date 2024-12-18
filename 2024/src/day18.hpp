/******************************************************************************
 * File:        day18.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-18
 *****************************************************************************/

#ifndef DAY18_HPP_GLFIEBZT
#define DAY18_HPP_GLFIEBZT

#include "ds/grid.hpp"
#include "graph_traversal.hpp" // for bfs
#include "lib.hpp"             // for expect_input, Pos, Delta, DIRECTIONS
#include <iostream>            // for istream
#include <vector>              // for vector
// IWYU pragma: no_include <initializer_list>  // for DIRECTIONS

namespace aoc::day18 {

int shortest_path_distance(const aoc::ds::Grid<bool> &grid) {
    const auto process_neighbors = [&grid](const Pos &pos, auto &&process) {
        for (auto dir : DIRECTIONS) {
            Pos neighbor = pos + Delta(dir, true);
            if (grid.in_bounds(neighbor) && grid[neighbor]) {
                process(neighbor);
            }
        }
    };

    const auto is_target = [](const Pos &pos) {
        return pos.x == 0 && pos.y == 0;
    };

    return aoc::graph::bfs(Pos{grid.width - 1, grid.height - 1},
                           process_neighbors, is_target, {});
}

std::vector<Pos> read_input(std::istream &is) {
    std::vector<Pos> corrupted_bytes;
    Pos pos;
    while (is >> pos.x >> aoc::expect_input(',') >> pos.y) {
        corrupted_bytes.push_back(pos);
    }
    return corrupted_bytes;
}

} // namespace aoc::day18

#endif /* end of include guard: DAY18_HPP_GLFIEBZT */
