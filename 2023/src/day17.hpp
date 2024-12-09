/******************************************************************************
 * File:        day17.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-17
 *****************************************************************************/

#ifndef DAY17_HPP_3D5Q7QMY
#define DAY17_HPP_3D5Q7QMY

#include "ds/grid.hpp"         // for Grid
#include "graph_traversal.hpp" // for dijkstra, a_star
#include "lib.hpp"             // for Pos, DEBUG, as_number
#include "util/hash.hpp"       // for make_hash

#include <algorithm>        // for transform
#include <compare>          // for strong_ordering
#include <cstddef>          // for size_t
#include <functional>       // for function, bind_front, hash
#include <initializer_list> // for initializer_list
#include <iostream>         // for cerr, istream, ostream
#include <iterator>         // for back_inserter
#include <string>           // for string, getline
#include <unordered_map>    // for unordered_map
#include <utility>          // for move, pair
#include <vector>           // for vector

namespace aoc::day17 {

enum Orientation : unsigned char {
    horizontal = 0,
    vertical = 1,
};

class CityMap {
  public:
    struct Key {
        Pos pos;
        Orientation orient;

        std::strong_ordering operator<=>(const Key &) const = default;
    };

  private:
    aoc::ds::Grid<unsigned char> block_costs;

    void process_neighbors(bool ultra, const Key &key, auto &&visit) const;
    int get_distance(const Key &from, const Key &to) const;

  public:
    explicit CityMap(std::vector<std::vector<unsigned char>> &&block_costs)
        : block_costs(std::move(block_costs)) {}
    template <aoc::Part part>
    int find_shortest_path() const;
    void print(std::ostream &, const std::vector<Key> &path = {}) const;
    static CityMap read(std::istream &is);
};

std::ostream &operator<<(std::ostream &os, const CityMap::Key &key) {
    os << "{" << key.pos << ", "
       << (key.orient == Orientation::horizontal ? "horizontal" : "vertical")
       << "}";
    return os;
}

void CityMap::process_neighbors(bool ultra, const Key &key,
                                auto &&visit) const {
    const int min_straight_moves = ultra ? 4 : 1;
    const int max_straight_moves = ultra ? 10 : 3;
    if (key.pos.x == 0 && key.pos.y == 0 &&
        key.orient == Orientation::horizontal) {
        // allow going down initially
        visit({key.pos, Orientation::vertical});
    }
    for (const auto orient : {Orientation::horizontal, Orientation::vertical}) {
        if (orient == key.orient) {
            continue;
        }
        for (int dist = min_straight_moves; dist <= max_straight_moves;
             ++dist) {
            for (int dir : {-1, 1}) {
                Key neighbor{key.pos, orient};
                if (orient == Orientation::horizontal) {
                    neighbor.pos.x += dir * dist;
                } else {
                    neighbor.pos.y += dir * dist;
                }
                if (!block_costs.in_bounds(neighbor.pos)) {
                    continue;
                }
                visit(neighbor);
            }
        }
    }
}

int CityMap::get_distance(const Key &from, const Key &to) const {
    int total = 0;
    if (to.orient == Orientation::horizontal) {
        int delta = to.pos.x > from.pos.x ? -1 : 1;
        for (Pos pos = to.pos; pos.x != from.pos.x; pos.x += delta) {
            total += block_costs[pos];
        }
    } else {
        int delta = to.pos.y > from.pos.y ? -1 : 1;
        for (Pos pos = to.pos; pos.y != from.pos.y; pos.y += delta) {
            total += block_costs[pos];
        }
    }
    return total;
}

template <aoc::Part part>
int CityMap::find_shortest_path() const {
    constexpr ultra = part == PART_2;
    Key source{Pos(0, 0), Orientation::horizontal};
    const Pos target(block_costs.width - 1, block_costs.height - 1);
    std::function<void(const Key &, int)> visit = [](const Key &, int) {};
#if 0
    if constexpr (aoc::DEBUG && ultra) {
        visit = [this, ultra](const Key &key, int dist) {
            std::cerr << "visiting " << key << ", with distance=" << dist
                      << "\n";
            std::cerr << "neighbors:\n";
            for (const auto &neighbor : get_neighbors(ultra, key)) {
                std::cerr << "  " << neighbor
                          << ", dist=" << get_distance(key, neighbor) << "\n";
            }
        };
    }
#endif
    const auto is_target = [&target](const Key &key) -> bool {
        return key.pos == target;
    };
#if 1
    const auto &[distance, path] = aoc::graph::dijkstra(
        source,
        [this, ultra](const Key &key, auto &&visit_neighbor) {
            process_neighbors(ultra, key, visit_neighbor);
        },
        std::bind_front(&CityMap::get_distance, this), is_target, visit);
#else
    const auto &[distance, path] = aoc::graph::a_star(
        source,
        [this, ultra](const Key &key, auto &&visit_neighbor) {
            process_neighbors(ultra, key, visit_neighbor);
        },
        std::bind_front(&CityMap::get_distance, this), is_target,
        [&target](const Key &key) -> int {
            return (key.pos - target).manhattan_distance();
        },
        visit);
#endif
    if constexpr (aoc::DEBUG) {
        if (distance >= 0) {
            std::cerr << "found path with distance " << distance << ", length "
                      << path.size() << ":\n";
        } else {
            std::cerr << "no path found from " << source << " to " << target
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

void CityMap::print(std::ostream &os, const std::vector<Key> &path) const {
    std::unordered_map<Pos, const Key *> path_lookup;
    for (const Key &key : path) {
        path_lookup[key.pos] = &key;
    }

    Pos pos;
    for (pos.y = 0; pos.y < block_costs.height; ++pos.y) {
        for (pos.x = 0; pos.x < block_costs.width; ++pos.x) {
            auto it = path_lookup.find(pos);
            if (it != path_lookup.end()) {
                switch (it->second->orient) {
                case Orientation::horizontal:
                    os << '-';
                    break;
                case Orientation::vertical:
                    os << '|';
                    break;
                }
            } else {
                os << as_number{block_costs[pos]};
            }
        }
        os << '\n';
    }
}

CityMap CityMap::read(std::istream &is) {
    std::vector<std::vector<unsigned char>> block_costs;
    std::string line;
    while (std::getline(is, line)) {
        std::vector<unsigned char> costs;
        std::transform(line.begin(), line.end(), std::back_inserter(costs),
                       [](char ch) { return ch - '0'; });
        block_costs.push_back(std::move(costs));
    }
    return CityMap{std::move(block_costs)};
}

} // namespace aoc::day17

template <>
struct std::hash<aoc::day17::CityMap::Key> {
    std::size_t operator()(const aoc::day17::CityMap::Key &key) const noexcept {
        // random number
        std::size_t seed = 0x580ac2097baf0cb7ull;
        util::make_hash(seed, key.pos.x, key.pos.y,
                        key.orient == aoc::day17::Orientation::horizontal);
        return seed;
    }
};

#endif /* end of include guard: DAY17_HPP_3D5Q7QMY */
