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
#include <set>         // for set
#include <string>      // for string, getline
#include <utility>     // for move
#include <vector>      // for vector

namespace aoc::day06 {

struct MapTile {
    unsigned char blocked;
    std::bitset<4> m_visited;

    explicit MapTile(bool blocked) : blocked(blocked) {}

    void mark_visited(AbsDirection dir) {
        m_visited.set(static_cast<std::underlying_type_t<AbsDirection>>(dir));
    }
    void clear_visited(AbsDirection dir) {
        m_visited.reset(static_cast<std::underlying_type_t<AbsDirection>>(dir));
    }
    bool is_visited() const { return m_visited.any(); }
    bool is_visited(AbsDirection dir) const {
        return m_visited[static_cast<std::underlying_type_t<AbsDirection>>(
            dir)];
    }
};

std::ostream &operator<<(std::ostream &os, const MapTile &tile) {
    if (tile.blocked) {
        os << (tile.blocked == 2 ? 'O' : '#');
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

    // positions for obstacles in part 2
    std::set<Pos> new_loop_obstacles;
    // used for faster loop checking (it's a member variable so the recursive
    // call to run() can reuse it)
    std::set<std::pair<Pos, AbsDirection>> loop_entrypoints;

  public:
    GuardSim(std::vector<std::vector<MapTile>> &&lab_map, Pos &&start_pos)
        : Grid<MapTile>(std::move(lab_map)), guard_pos(std::move(start_pos)),
          guard_dir(AbsDirection::north) {}

    /// returns true if a loop was entered
    bool run(bool check_obstacles = true);
    int count_visited() const;
    int count_new_loops() const { return new_loop_obstacles.size(); }

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

bool GuardSim::run(bool check_obstacles) {
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
            return true;
        }
        curr_tile.mark_visited(guard_dir);
        Pos new_pos = guard_pos + Delta(guard_dir, true);
        AbsDirection turn_dir =
            aoc::directions::turn(guard_dir, RelDirection::right);
        if (in_bounds(new_pos) && (*this)[new_pos].blocked) {
            // mark all positions behind the guard until an obstacle as
            // potential entry points for a loop
            if (check_obstacles) {
                Delta d(guard_dir, true);
                Pos prev_pos = guard_pos - d;
                while (in_bounds(prev_pos) && !(*this)[prev_pos].blocked) {
                    loop_entrypoints.emplace(prev_pos, guard_dir);
                    prev_pos -= d;
                }
            }
            // turn right
            guard_dir = turn_dir;
        } else {
            // continue straight
            if (check_obstacles && in_bounds(new_pos)) {
                MapTile &new_tile = (*this)[new_pos];
                if (new_tile.is_visited()) {
                    // placing an obstacle here would block the previous path
                } else if (loop_entrypoints.contains({guard_pos, turn_dir})) {
                    // fast path to avoid copying the entire map
                    new_loop_obstacles.insert(new_pos);
                    if constexpr (aoc::DEBUG) {
                        std::cerr << "map with new obstacle:\n";
                        new_tile.blocked = 2;
                        std::cerr << *this;
                        // cppcheck-suppress redundantAssignment
                        new_tile.blocked = 0;
                        std::cerr << "adding an obstacle at " << new_pos
                                  << " would create a new loop; part 2 count = "
                                  << new_loop_obstacles.size() << "\n";
                    }
                } else {
                    // save the simulation state
                    auto orig_data = m_data;
                    auto orig_guard_pos = guard_pos;
                    auto orig_guard_dir = guard_dir;
                    // try adding an obstacle in front of the guard
                    if constexpr (aoc::DEBUG) {
                        std::cerr << "adding an obstacle at " << new_pos
                                  << " to see if the guard enters a loop\n";
                    }
                    new_tile.blocked = 2;
                    curr_tile.clear_visited(guard_dir);
                    if (run(false)) {
                        // entered a loop
                        new_loop_obstacles.insert(new_pos);
                        if constexpr (aoc::DEBUG) {
                            std::cerr << "from full simulation, adding an "
                                         "obstacle at "
                                      << new_pos
                                      << " creates a new loop; part 2 count = "
                                      << new_loop_obstacles.size() << "\n";
                        }
                    } else if constexpr (aoc::DEBUG) {
                        std::cerr << "guard did not enter a loop\n";
                    }
                    // restore the simulation state
                    m_data = std::move(orig_data);
                    guard_pos = std::move(orig_guard_pos);
                    guard_dir = orig_guard_dir;
                }
            }
            guard_pos = new_pos;
        }
    }
    if constexpr (aoc::DEBUG) {
        std::cerr << "exited the map at " << guard_pos << " facing "
                  << guard_dir << "\n";
    }
    return false;
}

int GuardSim::count_visited() const {
    return std::count_if(m_data.begin(), m_data.end(),
                         [](const MapTile &tile) { return tile.is_visited(); });
}

std::ostream &operator<<(std::ostream &os, const GuardSim &sim) {
    return sim.custom_print(os, [&os, &sim](auto &tile, const Pos &pos) {
        char ch = '.';
        if (tile.is_visited()) {
            using enum AbsDirection;
            bool vert = tile.is_visited(north) || tile.is_visited(south);
            bool horz = tile.is_visited(east) || tile.is_visited(west);
            if (vert && horz) {
                ch = '+';
            } else {
                ch = vert ? '|' : '-';
            }
        }
        if (tile.blocked) {
            ch = tile.blocked == 2 ? 'O' : '#';
        }
        if (sim.guard_pos == pos) {
            switch (sim.guard_dir) {
            case AbsDirection::north:
                ch = '^';
                break;
            case AbsDirection::east:
                ch = '>';
                break;
            case AbsDirection::south:
                ch = 'v';
                break;
            case AbsDirection::west:
                ch = '<';
                break;
            }
        }
        os << ch;
    });
}

} // namespace aoc::day06

#endif /* end of include guard: DAY06_HPP_FZKH8ZND */
