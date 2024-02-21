/******************************************************************************
 * File:        day16.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-17
 *****************************************************************************/

#ifndef DAY16_HPP_80LYO54U
#define DAY16_HPP_80LYO54U

#include "ds/grid.hpp" // for Grid
#include "graph_traversal.hpp"
#include "lib.hpp"          // for Pos, AbsDirection, Delta
#include <algorithm>        // for count_if
#include <cassert>          // for assert
#include <compare>          // for strong_ordering
#include <cstddef>          // for size_t
#include <functional>       // for bind_front
#include <initializer_list> // for initializer_list
#include <iostream>         // for istream, ostream
#include <map>              // for map
#include <set>              // for set
#include <string>           // for string, getline
#include <tuple>            // for tie
#include <utility>          // for pair, make_pair
#include <vector>           // for vector

namespace aoc::day16 {

enum class TileType : char {
    empty = '.',
    forward_mirror = '/',
    backward_mirror = '\\',
    vert_splitter = '|',
    horz_splitter = '-',
};

struct Tile {
    TileType type;
    bool energized;

    std::vector<AbsDirection> get_out_dir(AbsDirection in_dir) const;
};

std::vector<AbsDirection> Tile::get_out_dir(AbsDirection in_dir) const {
    using enum AbsDirection;
    AbsDirection out_dir = in_dir;
    switch (type) {
    case TileType::empty:
        break;
    case TileType::vert_splitter:
        if (in_dir == east || in_dir == west) {
            return {north, south};
        }
        break;
    case TileType::horz_splitter:
        if (in_dir == north || in_dir == south) {
            return {east, west};
        }
        break;
    case TileType::forward_mirror:
        switch (in_dir) {
        case north:
            out_dir = east;
            break;
        case south:
            out_dir = west;
            break;
        case east:
            out_dir = north;
            break;
        case west:
            out_dir = south;
            break;
        }
        break;
    case TileType::backward_mirror:
        switch (in_dir) {
        case north:
            out_dir = west;
            break;
        case south:
            out_dir = east;
            break;
        case east:
            out_dir = south;
            break;
        case west:
            out_dir = north;
            break;
        }
        break;
    }
    return {out_dir};
}

class LaserGrid : public aoc::ds::Grid<Tile> {
    using visit_cache_t = std::set<std::pair<Pos, AbsDirection>>;

    void send_beam_helper(Pos pos, AbsDirection dir, visit_cache_t &seen);

  public:
    explicit LaserGrid(std::vector<std::vector<Tile>> &&tiles)
        : Grid<Tile>(std::move(tiles)) {}
    void send_beam(Pos pos, AbsDirection dir);
    void clear_energized();
    int count_energized() const;

    static LaserGrid read(std::istream &);

    void print_energized(std::ostream &os) const;
    friend std::ostream &operator<<(std::ostream &, const LaserGrid &);
};

void LaserGrid::send_beam(Pos pos, AbsDirection dir) {
    visit_cache_t seen;
    send_beam_helper(pos, dir, seen);
}

void LaserGrid::send_beam_helper(Pos pos, AbsDirection dir,
                                 visit_cache_t &seen) {
    while (in_bounds(pos)) {
        auto key = std::make_pair(pos, dir);
        if (seen.contains(key)) {
            return;
        }
        seen.insert(key);
        Tile &tile = (*this)[pos];
        tile.energized = true;
        std::vector<AbsDirection> new_dirs = tile.get_out_dir(dir);
        assert(new_dirs.size() > 0);
        // recurse on any extra beams
        for (std::size_t i = 1; i < new_dirs.size(); ++i) {
            send_beam_helper(pos + Delta(new_dirs[i], true), new_dirs[i], seen);
        }
        dir = new_dirs[0];
        pos += Delta(dir, true);
    }
}

void LaserGrid::clear_energized() {
    for (auto &tile : data()) {
        tile.energized = false;
    }
}

int LaserGrid::count_energized() const {
    return std::count_if(m_data.begin(), m_data.end(),
                         [](const Tile &tile) { return tile.energized; });
}

LaserGrid LaserGrid::read(std::istream &is) {
    std::vector<std::vector<Tile>> tiles;
    std::string line;
    while (std::getline(is, line)) {
        tiles.emplace_back();
        for (char ch : line) {
            tiles.back().push_back(Tile{static_cast<TileType>(ch), false});
        }
    }
    return LaserGrid{std::move(tiles)};
}

void LaserGrid::print_energized(std::ostream &os) const {
    for (const auto &row : *this) {
        for (const auto &tile : row) {
            os << (tile.energized ? '#' : '.');
        }
        os << '\n';
    }
}

std::ostream &operator<<(std::ostream &os, const LaserGrid &laser_grid) {
    for (const auto &row : laser_grid) {
        for (const auto &tile : row) {
            os << static_cast<char>(tile.type);
        }
        os << '\n';
    }
    return os;
}

///

struct GraphHelper {
    struct Key {
        Pos pos;
        AbsDirection dir;

        Key(const Pos &pos, AbsDirection dir) : pos(pos), dir(dir) {}

        std::strong_ordering operator<=>(const Key &) const = default;
    };

    const LaserGrid &grid;
    std::set<Key> sources;
    std::vector<std::vector<Key>> components;
    std::vector<std::vector<int>> component_successors;
    std::map<Key, int> source_components;

    std::vector<Key> get_neighbors(const Key &key) const {
        std::vector<Key> neighbors;
        for (AbsDirection dir : grid[key.pos].get_out_dir(key.dir)) {
            Pos new_pos = key.pos + Delta(dir, true);
            if (grid.in_bounds(new_pos)) {
                neighbors.emplace_back(new_pos, dir);
            }
        }
        return neighbors;
    }

    explicit GraphHelper(const LaserGrid &grid_) : grid(grid_) {
        Pos pos(0, 0);
        for (pos.y = 0; pos.y < grid.height; ++pos.y) {
            // this uses std::initializer_list
            for (auto dir :
                 {aoc::AbsDirection::east, aoc::AbsDirection::west}) {
                pos.x = dir == aoc::AbsDirection::east ? 0 : grid.width - 1;
                sources.emplace(pos, dir);
            }
        }
        for (pos.x = 0; pos.x < grid.width; ++pos.x) {
            for (auto dir :
                 {aoc::AbsDirection::south, aoc::AbsDirection::north}) {
                pos.y = dir == aoc::AbsDirection::south ? 0 : grid.height - 1;
                sources.emplace(pos, dir);
            }
        }

        std::set<std::pair<int, int>> edges;
        std::tie(components, edges) = aoc::graph::tarjan_scc<Key>(
            sources, std::bind_front(&GraphHelper::get_neighbors, this));

        component_successors.resize(components.size());
        for (const auto &[from, to] : edges) {
            component_successors[from].push_back(to);
        }

        for (int i = 0; const auto &component : components) {
            if (component.size() == 1 && sources.contains(*component.begin())) {
                source_components.emplace(*component.begin(), i);
            }
            ++i;
        }
    }

    const std::vector<int> &get_component_neighbors(int id) const {
        return component_successors[id];
    }

    int count_energized(const Key &source) const;
};

std::ostream &operator<<(std::ostream &os, const GraphHelper::Key &key) {
    os << "{" << key.pos << ", " << key.dir << "}";
    return os;
}

int GraphHelper::count_energized(const GraphHelper::Key &source) const {
    aoc::ds::Grid<bool> energized(grid, false);
    std::vector<bool> visited(components.size(), false);

    const auto visit_with_parent = [&energized, &components = components,
                                    &visited](int id, int, int) -> bool {
        if (visited[id]) {
            return false;
        }
        for (const Key &key : components[id]) {
            energized[key.pos] = true;
        }
        visited[id] = true;
        return true;
    };

    // handled in visit_with_parent() using a vector instead of an unordered_set
    constexpr bool use_seen = false;
    aoc::graph::dfs<use_seen>(
        source_components.at(source),
        std::bind_front(&GraphHelper::get_component_neighbors, this),
        visit_with_parent);

    return std::count(energized.data().begin(), energized.data().end(), true);
}

} // namespace aoc::day16

#endif /* end of include guard: DAY16_HPP_80LYO54U */
