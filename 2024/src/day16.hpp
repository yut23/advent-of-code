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
#include "graph_traversal.hpp" // for dijkstra
#include "lib.hpp" // for Pos, Delta, AbsDirection, RelDirection, DEBUG, read_lines
#include "util/hash.hpp" // for make_hash

#include <cassert>       // for assert
#include <compare>       // for strong_ordering
#include <cstddef>       // for size_t
#include <functional>    // for bind_front, hash
#include <iostream>      // for istream, ostream, cerr
#include <string>        // for string
#include <unordered_map> // for unordered_map
#include <utility>       // for move
#include <vector>        // for vector

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

  public:
    int find_shortest_path() const;
    void print(std::ostream &, const std::vector<Key> &path = {}) const;

    explicit Maze(std::vector<std::string> &&grid_);
    static Maze read(std::istream &is) { return Maze{aoc::read_lines(is)}; }
};

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

int Maze::find_shortest_path() const {
    const auto is_target = [&end_pos = end_pos](const Key &key) -> bool {
        return key.pos == end_pos;
    };

    const auto &[distance, path] = aoc::graph::dijkstra(
        Key{start_pos, AbsDirection::east},
        [this](const Key &key, auto &&visit_neighbor) {
            this->process_neighbors(key, visit_neighbor);
        },
        std::bind_front(&Maze::get_distance, this), is_target, /*visit*/ {});

    if constexpr (aoc::DEBUG) {
        if (distance >= 0) {
            std::cerr << "found path with distance " << distance << ", length "
                      << path.size() << ":\n";
        } else {
            std::cerr << "no path found from " << start_pos << " to " << end_pos
                      << "\n";
        }
        for (const Key &key : path) {
            std::cerr << "  " << key << "\n";
        }
        std::cerr << "\n";
        print(std::cerr, path);
        std::cerr << "\n";
    }

    return distance;
}

void Maze::print(std::ostream &os, const std::vector<Key> &path) const {
    std::unordered_map<Pos, const Key *> path_lookup;
    for (const Key &key : path) {
        path_lookup[key.pos] = &key;
    }

    Pos pos;
    for (pos.y = 0; pos.y < grid.height; ++pos.y) {
        for (pos.x = 0; pos.x < grid.width; ++pos.x) {
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
                os << grid[pos];
            }
        }
        os << '\n';
    }
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

template <>
struct std::hash<aoc::day16::Maze::Key> {
    std::size_t operator()(const aoc::day16::Maze::Key &key) const noexcept {
        // random number (hexdump -n8 -e '"0x" 8/1 "%02x" "ull\n"'</dev/urandom)
        std::size_t seed = 0x2b139d1412006c1dull;
        util::make_hash(seed, key.pos.x, key.pos.y, key.dir);
        return seed;
    }
};

#endif /* end of include guard: DAY16_HPP_WM3C4GBV */
