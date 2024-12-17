/******************************************************************************
 * File:        day16.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-16
 *****************************************************************************/

// based on 2023 day 17 (../../2023/src/day17.hpp)

#ifndef DAY16_HPP_WM3C4GBV
#define DAY16_HPP_WM3C4GBV

#include "ds/grid.hpp"         // for Grid
#include "graph_traversal.hpp" // for bfs
#include "lib.hpp" // for Pos, Delta, AbsDirection, RelDirection, DEBUG, read_lines
#include "util/hash.hpp" // for make_hash

#include <algorithm>        // for for_each, count
#include <cassert>          // for assert
#include <compare>          // for strong_ordering
#include <cstddef>          // for size_t
#include <functional>       // for greater, hash
#include <initializer_list> // for initializer_list
#include <iostream>         // for istream, ostream
#include <queue>            // for priority_queue
#include <string>           // for string
#include <unordered_map>    // for unordered_map
#include <utility>          // for move, pair
#include <vector>           // for vector

namespace aoc::day16 {

class Maze {
  public:
    struct Key {
        Pos pos;
        AbsDirection dir;

        std::strong_ordering operator<=>(const Key &) const = default;
    };

  private:
    aoc::ds::Grid<char> grid;
    Pos start_pos;
    Pos end_pos;

    void process_neighbors(const Key &key, auto &&visit) const;
    int get_distance(const Key &from, const Key &to) const;
    bool is_target(const Key &key) const { return key.pos == end_pos; }

  public:
    struct DijkstraBackref {
        int distance;
        std::vector<Key> incoming;

        DijkstraBackref() : distance(0), incoming() {}
        DijkstraBackref(int distance, const Key &node)
            : distance(distance), incoming({node}) {}
    };

  private:
    std::unordered_map<Key, DijkstraBackref>
    dijkstra_multi(const Key &source) const;

  public:
    std::pair<int, int> find_shortest_paths() const;
    void print(std::ostream &, const std::vector<Key> &path = {}) const;

    explicit Maze(std::vector<std::string> &&grid_);
    static Maze read(std::istream &is) { return Maze{aoc::read_lines(is)}; }
};

} // namespace aoc::day16

template <>
struct std::hash<aoc::day16::Maze::Key> {
    std::size_t operator()(const aoc::day16::Maze::Key &key) const noexcept {
        // random number (hexdump -n8 -e '"0x" 8/1 "%02x" "ull\n"'</dev/urandom)
        std::size_t seed = 0x2b139d1412006c1dull;
        util::make_hash(seed, key.pos.x, key.pos.y, key.dir);
        return seed;
    }
};

namespace aoc::day16 {

std::ostream &operator<<(std::ostream &os, const Maze::Key &key) {
    os << "{" << key.pos << ", " << key.dir << "}";
    return os;
}

void Maze::process_neighbors(const Key &key, auto &&visit) const {
    // try moving straight
    Key neighbor{key.pos + Delta(key.dir, true), key.dir};
    if (grid.in_bounds(neighbor.pos) && grid[neighbor.pos] != '#') {
        visit(neighbor);
    }
    // try turning
    visit(Key{key.pos, directions::turn(key.dir, RelDirection::left)});
    visit(Key{key.pos, directions::turn(key.dir, RelDirection::right)});
}

int Maze::get_distance(const Key &from, const Key &to) const {
    if (from.dir == to.dir) {
        // moved forward
        return 1;
    } else {
        // turned 90 degrees
        return 1000;
    }
}

/**
 * Modified version of aoc::graph::dijkstra() that records all the incoming
 * nodes with the same distance instead of just the first, so we can rebuild
 * all possible shortest paths.
 */
std::unordered_map<Maze::Key, Maze::DijkstraBackref>
Maze::dijkstra_multi(const Key &source) const {
    std::unordered_map<Key, DijkstraBackref> distances{};

    using pq_key = std::pair<int, Key>;
    std::priority_queue<pq_key, std::vector<pq_key>, std::greater<pq_key>>
        frontier{};

    distances[source] = {0, source};
    frontier.emplace(0, source);

    while (!frontier.empty()) {
        auto [dist, current] = std::move(frontier.top());
        frontier.pop();
        if (dist != distances.at(current).distance) {
            continue;
        }
        if (is_target(current)) {
            break;
        }
        process_neighbors(current, [this, &distances, &frontier, dist = dist,
                                    &current = current](const Key &neighbor) {
            int new_distance = dist + get_distance(current, neighbor);
            auto it = distances.find(neighbor);
            if (it == distances.end() || new_distance <= it->second.distance) {
                if (it != distances.end()) {
                    if (new_distance < it->second.distance) {
                        // this path has a shorter distance than one we found
                        // before
                        it->second = DijkstraBackref(new_distance, current);
                    } else if (new_distance == it->second.distance) {
                        // this path has the same distance as the existing one
                        it->second.incoming.push_back(current);
                    }
                } else {
                    // we've never seen this node before
                    distances.try_emplace(
                        neighbor, DijkstraBackref(new_distance, current));
                }
                frontier.emplace(new_distance, neighbor);
            }
        });
    }
    return distances;
}

// returns {path length, number of tiles visited} (part 1, part 2)
std::pair<int, int> Maze::find_shortest_paths() const {
    aoc::ds::Grid<bool> visited_tiles{grid, false};
    const auto distances = dijkstra_multi(Key{start_pos, AbsDirection::east});

    // reconstruct all possible paths
    const auto process_neighbors_back = [&distances](const Key &key,
                                                     auto &&process) {
        const DijkstraBackref &backref = distances.at(key);
        std::for_each(backref.incoming.begin(), backref.incoming.end(),
                      process);
    };
    const auto visit = [&visited_tiles](const Key &key, int /*distance*/) {
        visited_tiles[key.pos] = true;
    };
    std::vector<Key> end_keys{};
    int distance = -1;
    for (auto dir : DIRECTIONS) {
        Key key{end_pos, dir};
        auto it = distances.find(key);
        if (it != distances.end()) {
            end_keys.push_back(key);
            distance = it->second.distance;
        }
    }
    aoc::graph::bfs(
        end_keys, process_neighbors_back, [](const Key &) { return false; },
        visit);

    // count up all the visited tiles
    int visit_count = std::count(visited_tiles.data().begin(),
                                 visited_tiles.data().end(), true);
    return {distance, visit_count};
}

void Maze::print(std::ostream &os, const std::vector<Key> &path) const {
    std::unordered_map<Pos, const Key *> path_lookup;
    for (const Key &key : path) {
        path_lookup[key.pos] = &key;
    }

    Pos pos;
    grid.custom_print(os, [&path_lookup](auto &os, const Pos &pos, char value) {
        auto it = path_lookup.find(pos);
        if (it != path_lookup.end()) {
            switch (it->second->dir) {
            case AbsDirection::north:
                os << '^';
                break;
            case AbsDirection::east:
                os << '>';
                break;
            case AbsDirection::south:
                os << 'v';
                break;
            case AbsDirection::west:
                os << '<';
                break;
            }
        } else {
            os << value;
        }
    });
}

Maze::Maze(std::vector<std::string> &&grid_)
    : grid(std::move(grid_)), start_pos(-1, -1), end_pos(-1, -1) {
    grid.for_each_with_pos([this](const Pos &pos, char value) {
        if (value == 'S') {
            this->start_pos = pos;
        } else if (value == 'E') {
            this->end_pos = pos;
        }
    });
    assert(start_pos.x != -1 && start_pos.y != -1);
    assert(end_pos.x != -1 && end_pos.y != -1);
}

} // namespace aoc::day16

// vim: sw=4
#endif /* end of include guard: DAY16_HPP_WM3C4GBV */
