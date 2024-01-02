/******************************************************************************
 * File:        day21.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-21
 *****************************************************************************/

#ifndef DAY21_HPP_2X5T9V1Y
#define DAY21_HPP_2X5T9V1Y

#include "graph_traversal.hpp"
#include "lib.hpp"          // for AbsDirection, Pos, Delta
#include <functional>       // for bind_front
#include <initializer_list> // for initializer_list
#include <iostream>         // for istream, noskipws
#include <limits>           // for numeric_limits
#include <vector>           // for vector
// IWYU pragma: no_include <algorithm>  // for copy

namespace aoc::day21 {

class Farm {
    using Key = aoc::Pos;

    Pos start;
    std::vector<std::vector<bool>> grid;
    std::vector<std::vector<int>> distances;

    bool in_bounds(const Key &pos) const {
        return pos.y >= 0 && pos.x >= 0 &&
               pos.y < static_cast<int>(grid.size()) &&
               pos.x < static_cast<int>(grid[pos.y].size());
    }

    std::vector<Key> get_neighbors(const Key &key) {
        std::vector<Key> neighbors{};
        for (const AbsDirection &dir :
             {AbsDirection::north, AbsDirection::east, AbsDirection::south,
              AbsDirection::west}) {
            Pos pos = key + Delta(dir, true);
            if (in_bounds(pos) && grid[pos.y][pos.x]) {
                neighbors.push_back(pos);
            }
        }
        return neighbors;
    }

    bool is_target(int target_distance, const Key &key) const {
        return distances[key.y][key.x] > target_distance;
    }

    void visit(const Key &key, int distance) {
        distances[key.y][key.x] = distance;
    }

  public:
    int part_1();
    static Farm read(std::istream &is);
};

int Farm::part_1() {
    int target_distance = 64;

    aoc::graph::bfs(start, std::bind_front(&Farm::get_neighbors, this),
                    std::bind_front(&Farm::is_target, this, target_distance),
                    std::bind_front(&Farm::visit, this));

    int count = 0;
    for (const auto &row : distances) {
        for (const auto &distance : row) {
            if (distance <= target_distance &&
                distance % 2 == target_distance % 2) {
                ++count;
            }
        }
    }

    return count;
}

Farm Farm::read(std::istream &is) {
    Farm farm;
    is >> std::noskipws;
    char ch;
    for (int y = 0; is; ++y) {
        farm.grid.emplace_back();
        farm.distances.emplace_back();
        for (int x = 0; is >> ch && ch != '\n'; ++x) {
            farm.grid.back().emplace_back(ch != '#');
            farm.distances.back().emplace_back(
                ch == 'S' ? 0 : std::numeric_limits<int>::max());
            if (ch == 'S') {
                farm.start = {x, y};
            }
        }
    }
    return farm;
}

} // namespace aoc::day21

#endif /* end of include guard: DAY21_HPP_2X5T9V1Y */
