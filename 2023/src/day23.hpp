/******************************************************************************
 * File:        day23.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-24
 *****************************************************************************/

#ifndef DAY23_HPP_VAEIOPZT
#define DAY23_HPP_VAEIOPZT

#include "ds/grid.hpp" // for Grid
#include "graph_traversal.hpp"
#include "lib.hpp"          // for Pos, AbsDirection, Delta
#include <algorithm>        // for ranges::count
#include <cassert>          // for assert
#include <cstddef>          // for size_t
#include <functional>       // for bind_front
#include <initializer_list> // for initializer_list
#include <iostream>         // for istream, cerr
#include <map>              // for map
#include <queue>            // for queue
#include <string>           // for string, getline, basic_string
#include <unordered_set>    // for unordered_set (longest_path_dag)
#include <utility>          // for move, pair
#include <vector>           // for vector, erase_if

namespace aoc::day23 {

const std::map<char, AbsDirection> allowed_directions{
    {'>', AbsDirection::east},
    {'<', AbsDirection::west},
    {'^', AbsDirection::north},
    {'v', AbsDirection::south}};

class TrailMap {
    using Key = std::size_t;

    aoc::ds::Grid<char> grid;
    std::vector<std::vector<Key>> fwd_edges;
    std::vector<std::vector<std::pair<Key, int>>> undirected_edges;
    std::map<Pos, Key> key_lookup;
    std::vector<Pos> key_positions;

    Key start;
    Key target;

    bool get_grid_neighbors(const Pos &pos, const Pos &prev_pos,
                            std::vector<Pos> &neighbors);

    void add_edge(const Pos &from, const Pos &to, int distance);
    void construct_trails(Pos start_pos);

    int get_distance(const Key from, const Key to) const;

    Key pos_to_key(const Pos &pos);
    Pos key_to_pos(const Key key) const;

    const auto &get_fwd_neighbors(const Key key) const;
    const auto &get_undirected_neighbors(const Key key) const;

    void part_2_backtrack(const Key key, int distance, int depth,
                          std::basic_string<bool> &seen,
                          int &max_distance) const;

  public:
    explicit TrailMap(const std::vector<std::string> &grid_);
    static TrailMap read(std::istream &);
    int part_1() const;
    int part_2() const;
};

TrailMap::Key TrailMap::pos_to_key(const Pos &pos) {
    auto it = key_lookup.lower_bound(pos);
    if (it == key_lookup.end() || it->first != pos) {
        // add new key for pos
        it = key_lookup.emplace_hint(it, pos, key_positions.size());
        key_positions.push_back(pos);
        fwd_edges.resize(key_positions.size());
        undirected_edges.resize(key_positions.size());
    }
    return it->second;
}

Pos TrailMap::key_to_pos(const Key key) const { return key_positions[key]; }

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
    Pos start_pos{1, 0};
    assert(grid[start_pos] == '.');
    start = pos_to_key(start_pos);
    // ending point
    Pos target_pos{grid.width - 2, grid.height - 1};
    assert(grid[target_pos] == '.');
    target = pos_to_key(target_pos);
    construct_trails(start_pos);

    const auto &target_neighbors = undirected_edges[target];
    if (target_neighbors.size() == 1) {
        // remove other outgoing edges from the node right before the target,
        // as that's the only way to get to the target
        Key pentultimate = target_neighbors[0].first;
        std::erase_if(undirected_edges[pentultimate],
                      [target = target](const std::pair<Key, int> &entry) {
                          return entry.first != target;
                      });
    }
}

bool TrailMap::get_grid_neighbors(const Pos &pos, const Pos &prev_pos,
                                  std::vector<Pos> &neighbors) {
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
    Key from_key = pos_to_key(from);
    Key to_key = pos_to_key(to);
    assert(std::ranges::count(fwd_edges[from_key], to_key) == 0);
    fwd_edges[from_key].push_back(to_key);
    undirected_edges[from_key].emplace_back(to_key, distance);
    undirected_edges[to_key].emplace_back(from_key, distance);
}

void TrailMap::construct_trails(Pos start_pos) {
    std::queue<std::pair<Pos, Pos>> pending;
    pending.emplace(start_pos, start_pos);

    aoc::ds::Grid<bool> seen(grid, false);
    seen[start_pos] = true;
    // allocate this once and reuse it, instead of allocating and deallocating
    // inside the loop
    std::vector<Pos> neighbors;
    while (!pending.empty()) {
        auto [prev_pos, curr_pos] = pending.front();
        start_pos = prev_pos;
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
                    if (seen[neighbor]) {
                        continue;
                    }
                    pending.emplace(curr_pos, neighbor);
                    seen[neighbor] = true;
                }
                neighbors.clear();
                break;
            }
        }
        if (length > 0) {
            // add a path from start to curr
            add_edge(start_pos, curr_pos, length);
        }
    }
}

int TrailMap::get_distance(const Key from, const Key to) const {
    // a linear search here is fine, since there are at most 4 neighbors, and
    // only part 1 uses this function
    for (const auto &[key, dist] : undirected_edges[from]) {
        if (key == to) {
            return dist;
        }
    }
    assert(false);
}

int TrailMap::part_1() const {
    // find longest path in a DAG
    if constexpr (aoc::DEBUG) {
        std::cerr << "digraph G {\n";
        for (Key from_key = 0; const auto &neighbors : fwd_edges) {
            const Pos from = key_to_pos(from_key);
            for (const auto &to_key : neighbors) {
                const Pos to = key_to_pos(to_key);
                std::cerr << "  pos_" << from.x << "_" << from.y << " -> pos_"
                          << to.x << "_" << to.y
                          << " [label=" << get_distance(from_key, to_key)
                          << "];\n";
            }
            ++from_key;
        }
        std::cerr << "}\n";
    }
    const auto process_neighbors = [&fwd_edges = fwd_edges](const Key key,
                                                            auto &&visit) {
        std::ranges::for_each(fwd_edges[key], visit);
    };
    const auto is_target = [&target = target](const Key key) -> bool {
        return key == target;
    };
    const auto &[distance, path] = aoc::graph::longest_path_dag(
        start, process_neighbors,
        std::bind_front(&TrailMap::get_distance, this), is_target);

    if constexpr (aoc::DEBUG) {
        std::cerr << "longest path:\n";
        for (const auto &key : path) {
            std::cerr << key_to_pos(key) << "\n";
        }
    }
    return distance;
}

int TrailMap::part_2() const {
    // try brute-force backtracking DFS?
    // it runs in just over 1s on xrb in fast mode

    std::basic_string<bool> seen(undirected_edges.size(), false);
    int max_distance = 0;
    part_2_backtrack(start, 0, 0, seen, max_distance);

    return max_distance;
}

void TrailMap::part_2_backtrack(const Key key, int distance, int depth,
                                std::basic_string<bool> &seen,
                                int &max_distance) const {
    if (key == target) {
        if (distance > max_distance) {
            max_distance = distance;
        }
        return;
    }
    seen[key] = true;
    for (const auto &[neighbor, edge_length] : undirected_edges[key]) {
        if (seen[neighbor]) {
            continue;
        }
        part_2_backtrack(neighbor, distance + edge_length, depth + 1, seen,
                         max_distance);
    }
    seen[key] = false;
}
} // namespace aoc::day23

#endif /* end of include guard: DAY23_HPP_VAEIOPZT */
