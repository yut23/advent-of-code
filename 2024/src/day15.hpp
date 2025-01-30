/******************************************************************************
 * File:        day15.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-16
 *****************************************************************************/

#ifndef DAY15_HPP_7IJC3NYE
#define DAY15_HPP_7IJC3NYE

#include "ds/grid.hpp"                // for Grid
#include "unit_test/pretty_print.hpp" // for repr

#include "lib.hpp"       // for Pos, Delta, AbsDirection, DEBUG, FAST
#include <cassert>       // for assert
#include <functional>    // for hash (unordered_set)
#include <iostream>      // for istream
#include <optional>      // for optional
#include <string>        // for string, getline
#include <unordered_set> // for unordered_set
#include <utility>       // for swap, move, pair
#include <vector>        // for vector

namespace aoc::day15 {

class Warehouse {
    aoc::ds::Grid<char> grid;
    Pos robot_pos{-1, -1};

    Warehouse(int width, int height) : grid(width, height) {}

    std::optional<std::vector<Pos>> try_move(const Pos &pos, AbsDirection dir,
                                             bool wide_box = false) const;

  public:
    explicit Warehouse(const std::vector<std::string> &lines);
    Warehouse widen() const;

    void move_robot(AbsDirection dir);
    int gps_sum() const;

    friend std::ostream &operator<<(std::ostream &, const Warehouse &);
};

Warehouse::Warehouse(const std::vector<std::string> &lines) : grid(lines) {
    grid.for_each([this](char tile, const Pos &pos) {
        if (tile == '@') {
            this->robot_pos = pos;
        }
    });
}

Warehouse Warehouse::widen() const {
    Warehouse wider(grid.width * 2, grid.height);
    grid.for_each([&wider](char tile, const Pos &pos) {
        char &wider_left = wider.grid.at_unchecked(pos.x * 2, pos.y);
        char &wider_right = wider.grid.at_unchecked(pos.x * 2 + 1, pos.y);
        switch (tile) {
        case '#':
            wider_left = '#';
            wider_right = '#';
            break;
        case 'O':
            wider_left = '[';
            wider_right = ']';
            break;
        case '.':
            wider_left = '.';
            wider_right = '.';
            break;
        case '@':
            wider_left = '@';
            wider_right = '.';
            wider.robot_pos = {pos.x * 2, pos.y};
            break;
        }
    });
    return wider;
}

/**
 * Returns a collection of box positions that need to be updated, in order from
 * furthest to nearest, or an empty optional if the robot is blocked in that
 * direction
 */
std::optional<std::vector<Pos>>
Warehouse::try_move(const Pos &pos, AbsDirection dir, bool wide_box) const {
    using pretty_print::repr;
    Delta delta{dir, true};
    Pos new_pos = pos + delta;
    if (!grid.in_bounds(new_pos)) {
        // move is blocked
        return {};
    }
    char tile = grid[new_pos];
    if (tile == '#') {
        // move is blocked
        return {};
    }
    if (tile == '.') {
        // move is valid
        return {std::vector<Pos>{}};
    }
    if (tile == '[' || tile == ']') {
        // treat a wide box as two connected small boxes when moving vertically,
        // and recurse on each, setting the wide_box flag so we only do it once
        if (delta.dy != 0 && !wide_box) {
            Pos left_side, right_side;
            if (tile == '[') {
                left_side = pos;
                right_side = pos + Delta{1, 0};
            } else {
                left_side = pos + Delta{-1, 0};
                right_side = pos;
            }
            auto positions_left = try_move(left_side, dir, true);
            auto positions_right = try_move(right_side, dir, true);
            if (positions_left && positions_right) {
                // append all values from positions_right into positions_left
                positions_left->insert(positions_left->end(),
                                       positions_right->begin(),
                                       positions_right->end());
                return positions_left;
            } else {
                // movement was blocked somewhere
                return {};
            }
        } else {
            // this will handle horizontal movements as two unconnected boxes
            tile = 'O';
        }
    }
    if (tile == 'O') {
        auto positions = try_move(new_pos, dir);
        if (positions.has_value()) {
            // move is valid, add this position to the update list
            positions->push_back(new_pos);
        }
        return positions;
    }
    std::cerr << "unhandled tile type " << pretty_print::repr(tile) << " at "
              << new_pos << "\n";
    assert(false);
}

void Warehouse::move_robot(AbsDirection dir) {
    if (auto positions_to_move = try_move(robot_pos, dir);
        positions_to_move.has_value()) {
        Delta delta(dir, true);
        if constexpr (aoc::DEBUG) {
            std::cerr << "moving robot " << dir << " from " << robot_pos
                      << " to " << robot_pos + delta << "; box positions: "
                      << pretty_print::repr(*positions_to_move) << "\n";
        }
        // add the robot to the update list, after any boxes
        positions_to_move->push_back(robot_pos);
        // execute all movements, skipping duplicate positions
        std::unordered_set<Pos> already_moved;
        for (const Pos &pos : *positions_to_move) {
            Pos new_pos = pos + delta;
            bool inserted = already_moved.emplace(new_pos).second;
            if (inserted) {
                if constexpr (!aoc::FAST) {
                    assert(grid[new_pos] == '.');
                }
                std::swap(grid[pos], grid[new_pos]);
            }
        }
        // update the robot position
        robot_pos += delta;
    } else {
        if constexpr (aoc::DEBUG) {
            std::cerr << "blocked from moving " << dir << " at " << robot_pos
                      << "\n";
        }
    }
}

int Warehouse::gps_sum() const {
    int sum = 0;
    grid.for_each([&sum](char tile, const Pos &pos) {
        if (tile == 'O' || tile == '[') {
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
