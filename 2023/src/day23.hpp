/******************************************************************************
 * File:        day23.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-24
 *****************************************************************************/

#ifndef DAY23_HPP_VAEIOPZT
#define DAY23_HPP_VAEIOPZT

#include "lib.hpp"          // for Pos, AbsDirection, Delta
#include <cassert>          // for assert
#include <functional>       // for bind_front
#include <initializer_list> // for initializer_list
#include <iostream>         // for istream
#include <map>              // for map, multimap
#include <set>              // for set
#include <string>           // for string, getline
#include <utility>          // for move
#include <vector>           // for vector
// IWYU pragma: no_include <unordered_set>  // no clue why it wants this

#include "graph_traversal.hpp"

namespace aoc::day23 {

struct Trail {
    Pos start;
    Pos end;
    unsigned int length;
};

std::ostream &operator<<(std::ostream &os, const Trail &trail) {
    os << "Trail from " << trail.start << " to " << trail.end
       << ", length=" << trail.length;
    return os;
}

const std::map<char, AbsDirection> allowed_directions{
    {'>', AbsDirection::east},
    {'<', AbsDirection::west},
    {'^', AbsDirection::north},
    {'v', AbsDirection::south}};

class TrailMap {
    std::vector<std::string> grid;
    std::map<Pos, std::set<Pos>> grid_path;
    std::map<Pos, std::set<Pos>> grid_prev;

    std::map<Pos, Trail> trails;
    std::multimap<Pos, Pos> connections;

    bool in_bounds(const Pos &pos) const {
        return pos.y >= 0 && pos.x >= 0 &&
               pos.y < static_cast<int>(grid.size()) &&
               pos.x < static_cast<int>(grid[pos.y].size());
    }

    std::vector<Pos> get_grid_neighbors(const Pos &pos) const;

    Pos reconstruct_trails(Pos pos, const Pos &target);

  public:
    static TrailMap read(std::istream &);
    void parse_grid();
    int part_1() const;
};

TrailMap TrailMap::read(std::istream &is) {
    TrailMap map;
    std::string line;
    while (std::getline(is, line)) {
        map.grid.push_back(std::move(line));
    }
    return map;
}

std::vector<Pos> TrailMap::get_grid_neighbors(const Pos &pos) const {
    std::vector<Pos> neighbors;
    if (!in_bounds(pos)) {
        return {};
    }

    char terrain = grid[pos.y][pos.x];
    switch (terrain) {
    case '#':
        return {};
    case '>':
        return {pos + Delta(AbsDirection::east, true)};
    case '<':
        return {pos + Delta(AbsDirection::west, true)};
    case '^':
        return {pos + Delta(AbsDirection::north, true)};
    case 'v':
        return {pos + Delta(AbsDirection::south, true)};
    case '.':
        for (const auto dir : aoc::DIRECTIONS) {
            Pos new_pos = pos + Delta(dir, true);
            if (!in_bounds(new_pos)) {
                continue;
            }
            char new_terrain = grid[new_pos.y][new_pos.x];
            if (new_terrain == '#') {
                continue;
            }
            if (new_terrain != '.' &&
                dir != allowed_directions.at(new_terrain)) {
                continue;
            }
            auto it = grid_path.find(new_pos);
            if (it != grid_path.end() && it->second.contains(pos)) {
                continue;
            }
            // it = grid_path.find(pos);
            // if (it != grid_path.end() && it->second.contains(new_pos)) {
            //     continue;
            // }
            neighbors.push_back(new_pos);
        }
        assert(neighbors.size() <= 2);
        break;
    }
    return neighbors;
}

void TrailMap::parse_grid() {
    assert(grid.size() > 2 && grid[0].size() > 2);
    // starting point
    Pos start{1, 0};
    assert(grid[start.y][start.x] == '.');
    Pos target{static_cast<int>(grid[0].size()) - 2,
               static_cast<int>(grid.size()) - 1};
    assert(grid[target.y][target.x] == '.');

    auto visit_with_parent = [this](const Pos &pos, const Pos &parent, int) {
        if (parent != pos) {
            grid_path[parent].emplace(pos);
            grid_prev[pos].emplace(parent);
        }
    };
    aoc::graph::dfs<false>(start,
                           std::bind_front(&TrailMap::get_grid_neighbors, this),
                           visit_with_parent);

    if constexpr (aoc::DEBUG && false) {
        for (const auto &[parent, children] : grid_path) {
            std::cerr << parent << " -> {";
            for (auto it = children.begin(); it != children.end(); ++it) {
                if (it != children.begin()) {
                    std::cerr << ", ";
                }
                std::cerr << *it;
            }
            std::cerr << "}\n";
        }
    }

    Pos after_target = target + Delta(0, 1);
    trails.try_emplace(after_target, Trail{after_target, after_target, 0});
    reconstruct_trails(start, target);
    --trails.at(start).length;

    if constexpr (aoc::DEBUG) {
        std::cerr << "\n";
        Delta delta{1, 1};
        for (const auto &[_, trail] : trails) {
            std::cerr << trail << ": connected to {";
            auto range = connections.equal_range(trail.start);
            for (auto it = range.first; it != range.second; ++it) {
                if (it != range.first) {
                    std::cerr << ", ";
                }
                std::cerr << it->second + delta;
            }
            std::cerr << "}\n";
            if (in_bounds(trail.start)) {
                grid[trail.start.y][trail.start.x] = 'O';
            }
            if (in_bounds(trail.end)) {
                grid[trail.end.y][trail.end.x] = '*';
            }
        }

        std::cerr << "\n";
        for (const auto &l : grid) {
            std::cerr << l << "\n";
        }
    }
}

// reconstruct graph from path pointers
Pos TrailMap::reconstruct_trails(Pos pos, const Pos &target) {
    bool inserted = trails.try_emplace(pos, Trail{pos, pos, 1}).second;
    if (!inserted) {
        return pos;
    }
    Trail &trail = trails.at(pos);
    while (pos != target) {
        auto path_it = grid_path.find(pos);
        if (path_it == grid_path.end() || path_it->second.empty()) {
            break;
        }
        auto prev_it = grid_prev.find(pos);
        if (path_it->second.size() > 1 ||
            (prev_it != grid_prev.end() && prev_it->second.size() > 1)) {
            // multiple paths, recurse down each
            for (const Pos &next_pos : path_it->second) {
                Pos next_start = reconstruct_trails(next_pos, target);
                connections.emplace(trail.start, next_start);
            }
            break;
        }
        pos = *(path_it->second.begin());
        ++trail.length;
        trail.end = pos;
    }
    if (pos == target) {
        connections.emplace(trail.start, target + Delta(0, 1));
    }
    return trail.start;
}

int TrailMap::part_1() const {
    // find longest path via shortest path on graph with negated weights
    if constexpr (aoc::DEBUG) {
        std::cerr << "digraph G {\n";
        for (const auto &[p1, p2] : connections) {
            std::cerr << "  pos_" << p1.x << "_" << p1.y << " -> pos_" << p2.x
                      << "_" << p2.y << " [label=" << trails.at(p1).length
                      << "];\n";
        }
        std::cerr << "}\n";
    }

    Pos start{1, 0};
    Pos after_target{static_cast<int>(grid[0].size()) - 2,
                     static_cast<int>(grid.size())};
    auto get_neighbors = [this](const Pos &pos) -> std::vector<Pos> {
        const auto range = connections.equal_range(pos);
        std::vector<Pos> neighbors;
        for (auto it = range.first; it != range.second; ++it) {
            neighbors.emplace_back(it->second);
        }
        return neighbors;
    };
    auto get_distance = [this](const Pos &from, const Pos &) {
        return trails.at(from).length;
    };
    auto is_target = [&after_target](const Pos &pos) {
        return pos == after_target;
    };
    const auto &[distance, path] = aoc::graph::longest_path_dag(
        start, get_neighbors, get_distance, is_target);

    if constexpr (aoc::DEBUG) {
        std::cerr << "longest path:\n";
        for (const auto &pos : path) {
            std::cerr << trails.at(pos) << "\n";
        }
    }
    return distance;
}

} // namespace aoc::day23

#endif /* end of include guard: DAY23_HPP_VAEIOPZT */
