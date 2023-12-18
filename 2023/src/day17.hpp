/******************************************************************************
 * File:        day17.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-17
 *****************************************************************************/

#ifndef DAY17_HPP_3D5Q7QMY
#define DAY17_HPP_3D5Q7QMY

#include "graph_traversal.hpp" // for dijkstra
#include "lib.hpp" // for Pos, AbsDirection, Delta, opposite, DEBUG, as_number

#include <algorithm>     // for transform
#include <functional>    // for function, bind_front
#include <iostream>      // for cerr, istream, ostream
#include <iterator>      // for back_inserter
#include <optional>      // for optional
#include <string>        // for string, getline
#include <unordered_map> // for unordered_map
#include <utility>       // for move, pair
#include <vector>        // for vector

namespace aoc::day17 {

class CityMap {
  public:
    struct Key {
        Pos pos;
        AbsDirection dir;
        int move_count;

        auto operator<=>(const Key &) const = default;
    };

    std::vector<std::vector<unsigned char>> block_costs;

    std::vector<Key> get_neighbors(const Key &key) const;
    int get_distance(const Key &from, const Key &to) const;
    int height() const { return block_costs.size(); }
    int width() const { return block_costs[0].size(); }

    bool in_bounds(const Pos &pos) const {
        return pos.y >= 0 && pos.x >= 0 &&
               pos.y < static_cast<int>(block_costs.size()) &&
               pos.x < static_cast<int>(block_costs[pos.y].size());
    }

  public:
    int find_shortest_path() const;
    void print(std::ostream &, const std::vector<Key> &path = {}) const;
    static CityMap read(std::istream &is);

    friend std::ostream &operator<<(std::ostream &, const Key &);
};

std::ostream &operator<<(std::ostream &os, const CityMap::Key &key) {
    os << "{" << key.pos << ", " << key.dir << ", " << key.move_count << "}";
    return os;
}

std::vector<CityMap::Key> CityMap::get_neighbors(const Key &key) const {
    std::vector<Key> neighbors;
    for (const auto dir : {AbsDirection::north, AbsDirection::east,
                           AbsDirection::south, AbsDirection::west}) {
        if (dir == opposite(key.dir)) {
            continue;
        }
        Key neighbor{key.pos + Delta(dir, true), dir, 1};
        if (!in_bounds(neighbor.pos)) {
            continue;
        }
        if (dir == key.dir) {
            neighbor.move_count = key.move_count + 1;
            // not allowed to move in the same direction three times in a row
            if (neighbor.move_count > 3) {
                continue;
            }
        }
        neighbors.push_back(std::move(neighbor));
    }
    return neighbors;
}

int CityMap::get_distance([[maybe_unused]] const Key &from,
                          const Key &to) const {
    return block_costs[to.pos.y][to.pos.x];
}

int CityMap::find_shortest_path() const {
    Key source{Pos(0, 0), AbsDirection::east, 0};
    Pos target(width() - 1, height() - 1);
    std::optional<std::function<void(const Key &, int)>> visit = {};
#if 0
    if constexpr (aoc::DEBUG) {
        visit = [this](const Key &key, int dist) {
            std::cerr << "visiting " << key << ", with distance=" << dist
                      << "\n";
            std::cerr << "neighbors:\n";
            for (const auto &neighbor : get_neighbors(key)) {
                std::cerr << "  " << neighbor
                          << ", dist=" << get_distance(key, neighbor) << "\n";
            }
        };
    }
#endif
    const auto &[distance, path] = aoc::graph::dijkstra<Key>(
        source, std::bind_front(&CityMap::get_neighbors, this),
        std::bind_front(&CityMap::get_distance, this),
        [&target](const Key &key) -> bool { return key.pos == target; }, visit);
    if constexpr (aoc::DEBUG) {
        std::cerr << "found path with distance " << distance << ", length "
                  << path.size() << ":\n";
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

    Pos pos(0, 0);
    for (pos.y = 0; pos.y < height(); ++pos.y) {
        for (pos.x = 0; pos.x < width(); ++pos.x) {
            auto it = path_lookup.find(pos);
            if (it != path_lookup.end() && it->second->move_count > 0) {
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
                os << as_number{block_costs[pos.y][pos.x]};
            }
        }
        os << '\n';
    }
}

CityMap CityMap::read(std::istream &is) {
    CityMap city_map;
    std::string line;
    while (std::getline(is, line)) {
        std::vector<unsigned char> costs;
        std::transform(line.begin(), line.end(), std::back_inserter(costs),
                       [](char ch) { return ch - '0'; });
        city_map.block_costs.push_back(std::move(costs));
    }
    return city_map;
}

} // namespace aoc::day17

#endif /* end of include guard: DAY17_HPP_3D5Q7QMY */
