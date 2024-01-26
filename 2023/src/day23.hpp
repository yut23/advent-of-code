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

    std::vector<Pos> get_grid_neighbors(const Pos &pos) const;

    void add_edge(const Pos &from, const Pos &to, int distance = 1);
    void remove_edge(const Pos &from, const Pos &to);
    void collapse_node(const Pos &pos);
    void simplify_trails();

    std::pair<Pos, Pos> dist_key(const Pos &from, const Pos &to) const {
        return std::minmax(from, to);
    }
    int get_distance(const Pos &from, const Pos &to) const {
        return distances.at(dist_key(from, to));
    }

  public:
    explicit TrailMap(const std::vector<std::string> &grid) : grid(grid) {}
    static TrailMap read(std::istream &);
    void parse_grid();
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

std::vector<Pos> TrailMap::get_grid_neighbors(const Pos &pos) const {
    std::vector<Pos> neighbors;

    char terrain = grid[pos];
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
            if (!grid.in_bounds(new_pos)) {
                continue;
            }
            char new_terrain = grid[new_pos];
            if (new_terrain == '#') {
                continue;
            }
            if (new_terrain != '.' &&
                dir != allowed_directions.at(new_terrain)) {
                continue;
            }
            auto it = grid_path.find(new_pos);
            if (it != grid_path.end() && it->second.contains(pos)) {
                // don't go backwards through edges we've already seen
                continue;
            }
            neighbors.push_back(new_pos);
        }
        assert(neighbors.size() <= 2);
        break;
    }
    return neighbors;
}

void TrailMap::add_edge(const Pos &from, const Pos &to, int distance) {
    grid_path[from].emplace(to);
    grid_prev[to].emplace(from);
    distances[dist_key(from, to)] = distance;
}

void TrailMap::remove_edge(const Pos &from, const Pos &to) {
    assert(distances.erase(dist_key(from, to)) == 1);
    // avoid UAF in case from and to are references into grid_path and grid_prev
    auto &fwd = grid_path.at(from);
    auto &rev = grid_prev.at(to);
    assert(fwd.erase(to) == 1);
    assert(rev.erase(from) == 1);
}

void TrailMap::parse_grid() {
    assert(grid.height > 2 && grid.width > 2);
    // starting point
    Pos start{1, 0};
    assert(grid[start] == '.');
    Pos target{grid.width - 2, grid.height - 1};
    assert(grid[target] == '.');

    auto visit_with_parent = [this](const Pos &pos, const Pos &parent, int) {
        if (parent != pos) {
            add_edge(parent, pos);
        }
    };
    aoc::graph::dfs<false>(start,
                           std::bind_front(&TrailMap::get_grid_neighbors, this),
                           visit_with_parent);

    simplify_trails();
}

void TrailMap::simplify_trails() {
    // iterate through all nodes and collapse those that have a single
    // predecessor and successor
    std::vector<Pos> nodes;
    for (const auto &[key, _] : grid_path) {
        nodes.push_back(key);
    }
    for (const Pos &pos : nodes) {
        const auto succ_it = grid_path.find(pos);
        const auto pred_it = grid_prev.find(pos);
        if (succ_it != grid_path.end() && succ_it->second.size() == 1 &&
            pred_it != grid_prev.end() && pred_it->second.size() == 1) {
            collapse_node(pos);
        }
    }
}

void TrailMap::collapse_node(const Pos &pos) {
    const Pos &before = *grid_prev.at(pos).begin();
    const Pos &after = *grid_path.at(pos).begin();
    int new_distance = get_distance(before, pos) + get_distance(pos, after);
    // add link between before and after
    add_edge(before, after, new_distance);
    // remove links to pos
    remove_edge(before, pos);
    remove_edge(pos, after);
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
