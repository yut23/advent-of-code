/******************************************************************************
 * File:        day16.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-17
 *****************************************************************************/

#ifndef DAY16_HPP_80LYO54U
#define DAY16_HPP_80LYO54U

#include "ds/grid.hpp" // for Grid
#include "lib.hpp"     // for Pos, AbsDirection, Delta
#include <algorithm>   // for count_if
#include <cassert>     // for assert
#include <cstddef>     // for size_t
#include <iostream>    // for istream, ostream
#include <set>         // for set
#include <string>      // for string, getline
#include <utility>     // for pair, make_pair
#include <vector>      // for vector

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

} // namespace aoc::day16

#endif /* end of include guard: DAY16_HPP_80LYO54U */
