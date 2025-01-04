/******************************************************************************
 * File:        day15.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-16
 *****************************************************************************/

#ifndef DAY15_HPP_7IJC3NYE
#define DAY15_HPP_7IJC3NYE

#include "ds/grid.hpp" // for Grid
#include "lib.hpp"     // for Pos, Delta, AbsDirection, DEBUG, FAST
#include <cassert>     // for assert
#include <iostream>    // for istream
#include <optional>    // for optional
#include <string>      // for string, getline
#include <utility>     // for swap, move, pair
#include <vector>      // for vector

namespace aoc::day15 {

class Warehouse {
    aoc::ds::Grid<char> grid;
    Pos robot_pos;

    std::optional<Pos> find_empty_tile(Pos pos, AbsDirection dir) const;

  public:
    explicit Warehouse(const std::vector<std::string> &lines);
    void move_robot(AbsDirection dir);
    int gps_sum() const;

    friend std::ostream &operator<<(std::ostream &, const Warehouse &);
};

Warehouse::Warehouse(const std::vector<std::string> &lines)
    : grid(lines), robot_pos(-1, -1) {
    grid.for_each([this](char tile, const Pos &pos) {
        if (tile == '@') {
            this->robot_pos = pos;
        }
    });
}

/**
 * Returns the position directly in front of the robot if it's unoccupied, the
 * new position for a pushed box if there's one or more in the way, or an empty
 * optional if the robot is blocked in that direction.
 */
std::optional<Pos> Warehouse::find_empty_tile(Pos pos, AbsDirection dir) const {
    Delta delta{dir, true};
    for (pos += delta; grid.in_bounds(pos); pos += delta) {
        switch (grid[pos]) {
        case '.':
            // found an empty tile
            return pos;
        case '#':
            // hit a wall
            return {};
        }
    }
    // hit the edge of the map
    return {};
}

void Warehouse::move_robot(AbsDirection dir) {
    if (auto empty_pos = find_empty_tile(robot_pos, dir)) {
        // new position for the robot
        Pos new_pos = robot_pos + Delta(dir, true);
        if constexpr (aoc::DEBUG) {
            std::cerr << "moving robot " << dir << " from " << robot_pos
                      << " to " << new_pos;
        }
        if (grid[new_pos] == 'O') {
            // need to move the box to the next empty position
            if constexpr (!aoc::FAST) {
                assert(grid[*empty_pos] == '.');
            }
            if constexpr (aoc::DEBUG) {
                int box_count = (*empty_pos - new_pos).manhattan_distance();
                std::cerr << " and pushing " << box_count << " "
                          << (box_count > 1 ? "boxes" : "box") << " to "
                          << *empty_pos;
            }
            std::swap(grid[*empty_pos], grid[new_pos]);
        }
        // nothing in the way, just move the robot
        if constexpr (!aoc::FAST) {
            assert(grid[new_pos] == '.');
        }
        std::swap(grid[robot_pos], grid[new_pos]);
        robot_pos = new_pos;
    } else {
        if constexpr (aoc::DEBUG) {
            std::cerr << "blocked from moving " << dir << " at " << robot_pos;
        }
    }
    if constexpr (aoc::DEBUG) {
        std::cerr << "\n";
    }
}

int Warehouse::gps_sum() const {
    int sum = 0;
    grid.for_each([&sum](char tile, const Pos &pos) {
        if (tile == 'O') {
            sum += 100 * pos.y + pos.x;
        }
    });
    return sum;
}

std::pair<Warehouse, std::vector<AbsDirection>> read_input(std::istream &is) {
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(is, line)) {
        if (line.empty()) {
            break;
        }
        lines.push_back(std::move(line));
    }
    Warehouse warehouse{lines};
    // read robot moves
    char ch;
    std::vector<AbsDirection> moves;
    while (is >> ch) {
        switch (ch) {
        case '^':
            moves.push_back(AbsDirection::north);
            break;
        case '>':
            moves.push_back(AbsDirection::east);
            break;
        case 'v':
            moves.push_back(AbsDirection::south);
            break;
        case '<':
            moves.push_back(AbsDirection::west);
            break;
        }
    }
    return {warehouse, moves};
}

std::ostream &operator<<(std::ostream &os, const Warehouse &warehouse) {
    return warehouse.grid.custom_print(os, [&os](char tile) { os << tile; });
}

} // namespace aoc::day15

#endif /* end of include guard: DAY15_HPP_7IJC3NYE */
