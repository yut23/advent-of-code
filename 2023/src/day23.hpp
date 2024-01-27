/******************************************************************************
 * File:        day23.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-24
 *****************************************************************************/

#ifndef DAY23_HPP_VAEIOPZT
#define DAY23_HPP_VAEIOPZT

#include "ds/grid.hpp"      // for Grid
#include "lib.hpp"          // for Pos, AbsDirection, Delta
#include <algorithm>        // for minmax
#include <cassert>          // for assert
#include <functional>       // for bind_front
#include <initializer_list> // for initializer_list
#include <iostream>         // for istream
#include <map>              // for map
#include <set>              // for set
#include <string>           // for string, getline
#include <utility>          // for move
#include <vector>           // for vector

#include "graph_traversal.hpp"

namespace aoc::day23 {

const std::map<char, AbsDirection> allowed_directions{
    {'>', AbsDirection::east},
    {'<', AbsDirection::west},
    {'^', AbsDirection::north},
    {'v', AbsDirection::south}};

class TrailMap {
    aoc::ds::Grid<char> grid;
    std::map<Pos, std::set<Pos>> grid_path;
    std::map<Pos, std::set<Pos>> grid_prev;
    std::map<std::pair<Pos, Pos>, int> distances;

    bool get_grid_neighbors(const Pos &pos, const Pos &prev_pos,
                            std::vector<Pos> &neighbors) const;

    void add_edge(const Pos &from, const Pos &to, int distance);
    void construct_trails(Pos start);

    std::pair<Pos, Pos> dist_key(const Pos &from, const Pos &to) const {
        return std::minmax(from, to);
    }
    int get_distance(const Pos &from, const Pos &to) const {
        return distances.at(dist_key(from, to));
    }

  public:
    explicit TrailMap(const std::vector<std::string> &grid_);
    static TrailMap read(std::istream &);
    int part_1() const;
};

TrailMap TrailMap::read(std::istream &is) {
    std::vector<std::string> grid;
    std::string line;
    while (std::getline(is, line)) {
        grid.push_back(std::move(line));
    }
    return TrailMap{grid};
}

TrailMap::TrailMap(const std::vector<std::string> &grid_) : grid(grid_) {
    assert(grid.height > 2 && grid.width > 2);
    // starting point
    Pos start{1, 0};
    assert(grid[start] == '.');
    construct_trails(start);
}

bool TrailMap::get_grid_neighbors(const Pos &pos, const Pos &prev_pos,
                                  std::vector<Pos> &neighbors) const {
    char terrain = grid[pos];
    int neighbor_count = 0;
    switch (terrain) {
    case '#':
        break;
    case '>':
        neighbors.push_back(pos + Delta(AbsDirection::east, true));
        break;
    case '<':
        neighbors.push_back(pos + Delta(AbsDirection::west, true));
        break;
    case '^':
        neighbors.push_back(pos + Delta(AbsDirection::north, true));
        break;
    case 'v':
        neighbors.push_back(pos + Delta(AbsDirection::south, true));
        break;
    case '.':
        for (const auto dir : aoc::DIRECTIONS) {
            Pos new_pos = pos + Delta(dir, true);
            if (!grid.in_bounds(new_pos)) {
                continue;
            }
            char new_terrain = grid[new_pos];
            if (new_terrain == '#') {
                continue;
            }
            ++neighbor_count;
            if (new_pos == prev_pos) {
                // don't go backward
                continue;
            }
            if (new_terrain != '.' &&
                dir != allowed_directions.at(new_terrain)) {
                continue;
            }
            neighbors.push_back(new_pos);
        }
        assert(neighbors.size() <= 2);
        break;
    }
    return neighbor_count > 2;
}

void TrailMap::add_edge(const Pos &from, const Pos &to, int distance) {
    grid_path[from].emplace(to);
    grid_prev[to].emplace(from);
    distances[dist_key(from, to)] = distance;
}

void TrailMap::construct_trails(Pos start) {
    std::queue<std::pair<Pos, Pos>> pending;
    pending.emplace(start, start);
    // allocate this once and reuse it, instead of allocating and deallocating
    // inside the loop
    std::vector<Pos> neighbors;
    while (!pending.empty()) {
        auto [prev_pos, curr_pos] = pending.front();
        start = prev_pos;
        pending.pop();
        int length = prev_pos == curr_pos ? 0 : 1;
        while (true) {
            const bool is_junction =
                get_grid_neighbors(curr_pos, prev_pos, neighbors);
            if (!is_junction && neighbors.size() == 1) {
                ++length;
                prev_pos = curr_pos;
                curr_pos = neighbors.front();
                neighbors.clear();
            } else {
                // recurse on each of the outgoing paths (if any)
                for (const Pos &neighbor : neighbors) {
                    pending.emplace(curr_pos, neighbor);
                }
                neighbors.clear();
                break;
            }
        }
        if (length > 0) {
            // add a path from start to pos
            add_edge(start, curr_pos, length);
        }
    }
}

int TrailMap::part_1() const {
    // find longest path in a DAG
    if constexpr (aoc::DEBUG) {
        std::cerr << "digraph G {\n";
        for (const auto &[from, neighbors] : grid_path) {
            for (const Pos &to : neighbors) {
                std::cerr << "  pos_" << from.x << "_" << from.y << " -> pos_"
                          << to.x << "_" << to.y
                          << " [label=" << get_distance(from, to) << "];\n";
            }
        }
        std::cerr << "}\n";
    }

    Pos start{1, 0};
    Pos target{grid.width - 2, grid.height - 1};
    const std::set<Pos> empty_set{};
    auto get_neighbors = [this,
                          &empty_set](const Pos &pos) -> const std::set<Pos> & {
        auto it = grid_path.find(pos);
        if (it == grid_path.end()) {
            return empty_set;
        }
        return it->second;
    };
    auto is_target = [&target](const Pos &pos) -> bool {
        return pos == target;
    };
    const auto &[distance, path] = aoc::graph::longest_path_dag(
        start, get_neighbors, std::bind_front(&TrailMap::get_distance, this),
        is_target);

    if constexpr (aoc::DEBUG) {
        std::cerr << "longest path:\n";
        for (const auto &pos : path) {
            std::cerr << pos << "\n";
        }
    }
    return distance;
}

} // namespace aoc::day23

#endif /* end of include guard: DAY23_HPP_VAEIOPZT */
