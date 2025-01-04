/******************************************************************************
 * File:        day18.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-18
 *****************************************************************************/

#ifndef DAY18_HPP_GLFIEBZT
#define DAY18_HPP_GLFIEBZT

#include "ds/grid.hpp"
#include "graph_traversal.hpp" // for dijkstra
#include "lib.hpp"             // for expect_input, Pos, Delta
#include <functional>          // for hash (unordered_set)
#include <iostream>            // for istream
#include <unordered_set>       // for unordered_set
#include <utility>             // for pair (unordered_set)
#include <vector>              // for vector

namespace aoc::day18 {

std::unordered_set<Pos> shortest_path(const aoc::ds::Grid<bool> &grid) {
    const auto process_neighbors = [&grid](const Pos &pos, auto &&process) {
        grid.manhattan_kernel(pos, [&](bool open, const Pos &neighbor) {
            if (open && neighbor != pos) {
                process(neighbor);
            }
        });
    };

    const auto is_target = [](const Pos &pos) {
        return pos.x == 0 && pos.y == 0;
    };

    const auto get_distance = [](const Pos &, const Pos &) { return 1; };

    auto path =
        aoc::graph::dijkstra(Pos{grid.width - 1, grid.height - 1},
                             process_neighbors, get_distance, is_target, {})
            .second;

    return std::unordered_set<Pos>(path.begin(), path.end());
}

Pos find_cutoff_pos(aoc::ds::Grid<bool> &grid, auto begin, auto end) {
    std::unordered_set<Pos> path = shortest_path(grid);

    for (auto it = begin; it != end; ++it) {
        if (!grid[*it]) {
            continue;
        }
        grid[*it] = false;
        if (path.contains(*it)) {
            // this byte blocked the current shortest path, so recalculate it
            path = shortest_path(grid);
            if (path.empty()) {
                // this position cut off the path to the exit
                return *it;
            }
        }
    }
    return {-1, -1};
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
