/******************************************************************************
 * File:        day06.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-06
 *****************************************************************************/

// similar to 2023 day 16 (../2023/src/day16.hpp)

#ifndef DAY06_HPP_FZKH8ZND
#define DAY06_HPP_FZKH8ZND

#include "ds/grid.hpp" // for Grid
#include "lib.hpp"     // for AbsDirection, Delta, Pos, DEBUG
#include <algorithm>   // for count_if
#include <bitset>      // for bitset
#include <iostream>    // for istream, ostream, cerr
#include <string>      // for string, getline
#include <utility>     // for move
#include <vector>      // for vector

namespace aoc::day06 {

class MapTile {
    std::bitset<5> data;

  public:
    explicit MapTile(bool blocked) { data.set(0, blocked); }

    void mark_visited(AbsDirection dir) {
        data.set(1 + static_cast<std::underlying_type_t<AbsDirection>>(dir));
    }
    bool blocked() const { return data[0]; }
    bool is_visited() const { return !data[0] && data.any(); }
    bool is_visited(AbsDirection dir) const {
        return data[1 + static_cast<std::underlying_type_t<AbsDirection>>(dir)];
    }
};

std::ostream &operator<<(std::ostream &os, const MapTile &tile) {
    if (tile.blocked()) {
        os << '#';
    } else if (tile.is_visited()) {
        os << 'X';
    } else {
        os << '.';
    }
    return os;
}

class GuardSim : public aoc::ds::Grid<MapTile> {
    Pos guard_pos;
    AbsDirection guard_dir;

  public:
    GuardSim(std::vector<std::vector<MapTile>> &&lab_map, Pos &&start_pos)
        : Grid<MapTile>(std::move(lab_map)), guard_pos(std::move(start_pos)),
          guard_dir(AbsDirection::north) {}

    void run();
    int count_visited() const;

    friend std::ostream &operator<<(std::ostream &, const GuardSim &);
};

GuardSim read_input(std::istream &is) {
    // read file line-by-line
    std::vector<std::vector<MapTile>> tiles;
    std::string line;
    Pos guard_pos;
    for (int y = 0; std::getline(is, line); ++y) {
        tiles.emplace_back();
        for (int x = 0; x < static_cast<int>(line.size()); ++x) {
            char ch = line[x];
            tiles.back().emplace_back(ch == '#');
            if (ch == '^') {
                guard_pos = Pos(x, y);
            }
        }
    }
    return GuardSim(std::move(tiles), std::move(guard_pos));
}

void GuardSim::run() {
    while (in_bounds(guard_pos)) {
        if constexpr (aoc::DEBUG) {
            std::cerr << "guard is at " << guard_pos << " facing " << guard_dir
                      << "\n";
            std::cerr << *this;
        }
        MapTile &curr_tile = (*this)[guard_pos];
        if (curr_tile.is_visited(guard_dir)) {
            // guard is in a loop
            if constexpr (aoc::DEBUG) {
                std::cerr << "loop detected, stopping\n";
            }
            break;
        }
        curr_tile.mark_visited(guard_dir);
        Pos new_pos = guard_pos + Delta(guard_dir, true);
        if (in_bounds(new_pos) && (*this)[new_pos].blocked()) {
            // turn right
            guard_dir = aoc::directions::turn(guard_dir, RelDirection::right);
        } else {
            guard_pos = new_pos;
        }
    }
    if constexpr (aoc::DEBUG) {
        if (!in_bounds(guard_pos)) {
            std::cerr << "exited the map at " << guard_pos << " facing "
                      << guard_dir << "\n";
        }
    }
}

int GuardSim::count_visited() const {
    return std::count_if(m_data.begin(), m_data.end(),
                         [](const MapTile &tile) { return tile.is_visited(); });
}

std::ostream &operator<<(std::ostream &os, const GuardSim &sim) {
    Pos pos(0, 0);
    for (pos.y = 0; const auto &row : sim) {
        for (pos.x = 0; const auto &tile : row) {
            if (sim.guard_pos == pos) {
                switch (sim.guard_dir) {
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
                os << tile;
            }
            ++pos.x;
        }
        os << "\n";
        ++pos.y;
    }
    return os;
}

} // namespace aoc::day06

#endif /* end of include guard: DAY06_HPP_FZKH8ZND */
