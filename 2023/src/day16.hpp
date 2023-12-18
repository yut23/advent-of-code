/******************************************************************************
 * File:        day16.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-17
 *****************************************************************************/

#ifndef DAY16_HPP_80LYO54U
#define DAY16_HPP_80LYO54U

#include "lib.hpp"   // for Pos, AbsDirection, Delta
#include <algorithm> // for count_if
#include <cassert>   // for assert
#include <cstddef>   // for size_t
#include <iostream>  // for istream, ostream
#include <set>       // for set
#include <string>    // for string, getline
#include <utility>   // for pair, make_pair
#include <vector>    // for vector

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
    AbsDirection out_dir = in_dir;
    switch (type) {
    case TileType::empty:
        break;
    case TileType::vert_splitter:
        if (in_dir == AbsDirection::east || in_dir == AbsDirection::west) {
            return {AbsDirection::north, AbsDirection::south};
        }
        break;
    case TileType::horz_splitter:
        if (in_dir == AbsDirection::north || in_dir == AbsDirection::south) {
            return {AbsDirection::east, AbsDirection::west};
        }
        break;
    case TileType::forward_mirror:
        switch (in_dir) {
        case AbsDirection::north:
            out_dir = AbsDirection::east;
            break;
        case AbsDirection::south:
            out_dir = AbsDirection::west;
            break;
        case AbsDirection::east:
            out_dir = AbsDirection::north;
            break;
        case AbsDirection::west:
            out_dir = AbsDirection::south;
            break;
        }
        break;
    case TileType::backward_mirror:
        switch (in_dir) {
        case AbsDirection::north:
            out_dir = AbsDirection::west;
            break;
        case AbsDirection::south:
            out_dir = AbsDirection::east;
            break;
        case AbsDirection::east:
            out_dir = AbsDirection::south;
            break;
        case AbsDirection::west:
            out_dir = AbsDirection::north;
            break;
        }
        break;
    }
    return {out_dir};
}

class Grid {
    using visit_cache_t = std::set<std::pair<Pos, AbsDirection>>;
    std::vector<std::vector<Tile>> tiles;

    bool in_bounds(const Pos &pos) const {
        return pos.y >= 0 && pos.x >= 0 &&
               pos.y < static_cast<int>(tiles.size()) &&
               pos.x < static_cast<int>(tiles[pos.y].size());
    }
    void send_beam_helper(Pos pos, AbsDirection dir, visit_cache_t &seen);

  public:
    void send_beam(Pos pos, AbsDirection dir);
    void clear_energized();
    int count_energized() const;

    int width() const { return tiles[0].size(); }
    int height() const { return tiles.size(); }

    static Grid read(std::istream &);

    void print_energized(std::ostream &os) const;
    friend std::ostream &operator<<(std::ostream &, const Grid &);
};

void Grid::send_beam(Pos pos, AbsDirection dir) {
    visit_cache_t seen;
    send_beam_helper(pos, dir, seen);
}

void Grid::send_beam_helper(Pos pos, AbsDirection dir, visit_cache_t &seen) {
    while (in_bounds(pos)) {
        auto key = std::make_pair(pos, dir);
        if (seen.contains(key)) {
            return;
        }
        seen.insert(key);
        Tile &tile = tiles[pos.y][pos.x];
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

void Grid::clear_energized() {
    for (auto &row : tiles) {
        for (auto &tile : row) {
            tile.energized = false;
        }
    }
}

int Grid::count_energized() const {
    int count = 0;
    for (const auto &row : tiles) {
        count += std::ranges::count_if(
            row, [](const Tile &tile) { return tile.energized; });
    }
    return count;
}

Grid Grid::read(std::istream &is) {
    Grid grid;
    std::string line;
    while (std::getline(is, line)) {
        grid.tiles.emplace_back();
        for (char ch : line) {
            grid.tiles.back().push_back(Tile{static_cast<TileType>(ch), false});
        }
    }
    return grid;
}

void Grid::print_energized(std::ostream &os) const {
    for (const auto &row : tiles) {
        for (const auto &tile : row) {
            os << (tile.energized ? '#' : '.');
        }
        os << '\n';
    }
}

std::ostream &operator<<(std::ostream &os, const Grid &grid) {
    for (const auto &row : grid.tiles) {
        for (const auto &tile : row) {
            os << static_cast<char>(tile.type);
        }
        os << '\n';
    }
    return os;
}

} // namespace aoc::day16

#endif /* end of include guard: DAY16_HPP_80LYO54U */
