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
#include <cstddef>          // for size_t
#include <functional>       // for bind_front
#include <initializer_list> // for initializer_list
#include <iostream>         // for istream
#include <map>              // for map
#include <queue>            // for queue
#include <string>           // for string, getline
#include <unordered_map>    // for unordered_map
#include <unordered_set>    // for unordered_set
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
    using Key = std::size_t;

    aoc::ds::Grid<char> grid;
    std::unordered_map<Key, std::unordered_set<Key>> fwd_edges;
    std::unordered_map<Key, std::unordered_set<Key>> undirected_edges;
    std::map<std::pair<Key, Key>, int> distances;

    Key start;
    Key target;

    bool get_grid_neighbors(const Key key, const Key prev_key,
                            std::vector<Key> &neighbors) const;

    void add_edge(const Key from, const Key to, int distance);
    void construct_trails(Key start_key);

    std::pair<Key, Key> dist_key(const Key from, const Key to) const {
        return std::minmax(from, to);
    }
    int get_distance(const Key from, const Key to) const {
        return distances.at(dist_key(from, to));
    }

    Key pos_to_key(const Pos &pos) const {
        return grid.get_index(pos.x, pos.y);
    }
    Pos key_to_pos(Key key) const { return grid.index_to_pos(key); }

  public:
    explicit TrailMap(const std::vector<std::string> &grid_);
    static TrailMap read(std::istream &);
    int part_1() const;
    int part_2() const;
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
    start = pos_to_key(Pos{1, 0});
    assert(grid[start] == '.');
    // ending point
    target = pos_to_key(Pos{grid.width - 2, grid.height - 1});
    assert(grid[target] == '.');
    construct_trails(start);

    const auto &target_neighbors = undirected_edges.at(target);
    if (target_neighbors.size() == 1) {
        // remove other outgoing edges from the node right before the target,
        // as that's the only way to get to the target
        Key pentultimate = *target_neighbors.begin();
        std::erase_if(
            undirected_edges.at(pentultimate),
            [target = target](const Key key) { return key != target; });
    }
}

bool TrailMap::get_grid_neighbors(const Key key, const Key prev_key,
                                  std::vector<Key> &neighbors) const {
    char terrain = grid[key];
    const Pos pos = key_to_pos(key);
    int neighbor_count = 0;
    switch (terrain) {
    case '#':
        break;
    case '>':
        neighbors.push_back(pos_to_key(pos + Delta(AbsDirection::east, true)));
        break;
    case '<':
        neighbors.push_back(pos_to_key(pos + Delta(AbsDirection::west, true)));
        break;
    case '^':
        neighbors.push_back(pos_to_key(pos + Delta(AbsDirection::north, true)));
        break;
    case 'v':
        neighbors.push_back(pos_to_key(pos + Delta(AbsDirection::south, true)));
        break;
    case '.':
        for (const auto dir : aoc::DIRECTIONS) {
            Pos new_pos = pos + Delta(dir, true);
            if (!grid.in_bounds(new_pos)) {
                continue;
            }
            Key new_key = pos_to_key(new_pos);
            char new_terrain = grid[new_key];
            if (new_terrain == '#') {
                continue;
            }
            ++neighbor_count;
            if (new_key == prev_key) {
                // don't go backward
                continue;
            }
            if (new_terrain != '.' &&
                dir != allowed_directions.at(new_terrain)) {
                continue;
            }
            neighbors.push_back(new_key);
        }
        assert(neighbors.size() <= 2);
        break;
    }
    return neighbor_count > 2;
}

void TrailMap::add_edge(const Key from, const Key to, int distance) {
    fwd_edges[from].emplace(to);
    undirected_edges[from].emplace(to);
    undirected_edges[to].emplace(from);
    distances[dist_key(from, to)] = distance;
}

void TrailMap::construct_trails(Key start_key) {
    std::queue<std::pair<Key, Key>> pending;
    pending.emplace(start_key, start_key);
    // allocate this once and reuse it, instead of allocating and deallocating
    // inside the loop
    std::vector<Key> neighbors;
    while (!pending.empty()) {
        auto [prev_key, curr_key] = pending.front();
        start_key = prev_key;
        pending.pop();
        int length = prev_key == curr_key ? 0 : 1;
        while (true) {
            const bool is_junction =
                get_grid_neighbors(curr_key, prev_key, neighbors);
            if (!is_junction && neighbors.size() == 1) {
                ++length;
                prev_key = curr_key;
                curr_key = neighbors.front();
                neighbors.clear();
            } else {
                // recurse on each of the outgoing paths (if any)
                for (const Key neighbor : neighbors) {
                    pending.emplace(curr_key, neighbor);
                }
                neighbors.clear();
                break;
            }
        }
        if (length > 0) {
            // add a path from start_key to current position
            add_edge(start_key, curr_key, length);
        }
    }
}

int TrailMap::part_1() const {
    // find longest path in a DAG
    if constexpr (aoc::DEBUG) {
        std::cerr << "digraph G {\n";
        for (const auto &[from_key, neighbors] : fwd_edges) {
            const Pos from = key_to_pos(from_key);
            for (const auto &to_key : neighbors) {
                const Pos to = key_to_pos(to_key);
                std::cerr << "  pos_" << from.x << "_" << from.y << " -> pos_"
                          << to.x << "_" << to.y
                          << " [label=" << get_distance(from_key, to_key)
                          << "];\n";
            }
        }
        std::cerr << "}\n";
    }

    const decltype(fwd_edges)::mapped_type empty_set{};
    const auto get_neighbors = [this, &empty_set](const Key key)
        -> const decltype(TrailMap::fwd_edges)::mapped_type & {
        auto it = fwd_edges.find(key);
        if (it != fwd_edges.end()) {
            return it->second;
        }
        return empty_set;
    };
    const auto is_target = [&target = target](const Key key) -> bool {
        return key == target;
    };
    const auto &[distance, path] = aoc::graph::longest_path_dag(
        start, get_neighbors, std::bind_front(&TrailMap::get_distance, this),
        is_target);

    if constexpr (aoc::DEBUG) {
        std::cerr << "longest path:\n";
        for (const auto &key : path) {
            std::cerr << key_to_pos(key) << "\n";
        }
    }
    return distance;
}

int TrailMap::part_2() const {
    // try brute-force DFS?
    // it runs in just over 1s on xrb in fast mode
    const decltype(undirected_edges)::mapped_type empty_set{};
    const auto get_neighbors = [this, &empty_set](const Key key)
        -> const decltype(TrailMap::undirected_edges)::mapped_type & {
        auto it = undirected_edges.find(key);
        if (it != undirected_edges.end()) {
            return it->second;
        }
        return empty_set;
    };

    aoc::ds::Grid<bool> seen(grid.width, grid.height, false);
    int max_distance = 0;
    const auto dfs = [this, &get_neighbors, &target = target, &seen,
                      &max_distance](auto &&rec, const Key key, int distance,
                                     int depth = 0) -> void {
        if (key == target) {
            if (distance > max_distance) {
                max_distance = distance;
            }
            return;
        }
        seen[key] = true;
        for (const Key neighbor : get_neighbors(key)) {
            if (seen[neighbor]) {
                continue;
            }
            rec(rec, neighbor, distance + get_distance(key, neighbor),
                depth + 1);
        }
        seen[key] = false;
    };

    dfs(dfs, start, 0);

    return max_distance;
}
} // namespace aoc::day23

#endif /* end of include guard: DAY23_HPP_VAEIOPZT */
