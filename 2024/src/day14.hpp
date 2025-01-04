/******************************************************************************
 * File:        day14.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-14
 *****************************************************************************/

#ifndef DAY14_HPP_5W9MWUPG
#define DAY14_HPP_5W9MWUPG

#include "ds/grid.hpp" // for Grid
#include "lib.hpp"     // for Pos, Delta, expect_input, DIRECTIONS
#include <algorithm>   // for max_element
#include <array>       // for array
#include <iostream>    // for istream, ostream, noskipws, ws
#include <utility>     // for move
#include <vector>      // for vector
// IWYU pragma: no_include <initializer_list>  // for DIRECTIONS
// IWYU pragma: no_include <string>  // for operator== (expect_input)

namespace aoc::day14 {

struct Robot {
    Pos pos{0, 0};
    Delta vel{0, 0};

    void update(const Pos &bounds);
};

std::istream &operator>>(std::istream &is, Robot &robot) {
    is >> std::noskipws;
    is >> aoc::expect_input("p=") >> robot.pos.x >> aoc::expect_input(',') >>
        robot.pos.y >> aoc::expect_input(" v=") >> robot.vel.dx >>
        aoc::expect_input(',') >> robot.vel.dy;
    is >> std::ws;
    return is;
}

void Robot::update(const Pos &bounds) {
    pos += vel;
    if (pos.x < 0) {
        pos.x += bounds.x;
    } else if (pos.x >= bounds.x) {
        pos.x -= bounds.x;
    }
    if (pos.y < 0) {
        pos.y += bounds.y;
    } else if (pos.y >= bounds.y) {
        pos.y -= bounds.y;
    }
}

class Robots {
    std::vector<Robot> robots;
    Pos bounds;

    aoc::ds::Grid<int> robot_counts;

    explicit Robots(const Pos &bounds)
        : robots(), bounds(bounds), robot_counts(bounds.x, bounds.y, 0) {}

  public:
    static Robots read(std::istream &is, const Pos &bounds);
    void update();
    int safety_factor() const;
    int largest_clump() const;

    friend std::ostream &operator<<(std::ostream &os, const Robots &robots);
};

Robots Robots::read(std::istream &is, const Pos &bounds) {
    Robot robot;
    Robots robots(bounds);
    while (is >> robot) {
        ++robots.robot_counts[robot.pos];
        robots.robots.push_back(std::move(robot));
    }
    return robots;
}

void Robots::update() {
    for (auto &robot : robots) {
        --robot_counts[robot.pos];
        robot.update(bounds);
        ++robot_counts[robot.pos];
    }
}

int Robots::safety_factor() const {
    std::array<int, 4> quadrants{};
    for (const auto &robot : robots) {
        int quad_idx = 0;
        if (robot.pos.x == bounds.x / 2 || robot.pos.y == bounds.y / 2) {
            // robot is exactly in the middle, so it doesn't count for any
            // quadrant
            continue;
        }
        if (robot.pos.x > bounds.x / 2) {
            quad_idx |= 1;
        }
        if (robot.pos.y > bounds.y / 2) {
            quad_idx |= 2;
        }
        ++quadrants[quad_idx];
    }

    return quadrants[0] * quadrants[1] * quadrants[2] * quadrants[3];
}

int Robots::largest_clump() const {
    std::vector<int> clumps;
    aoc::ds::Grid<int> clump_indices(robot_counts, -1);
    for (const Robot &robot : robots) {
        // This isn't actually correct, and mislabels shapes with indents like
        //  ..##
        //  .##.
        // but it's good enough to find a Christmas tree. See day 12 for a
        // proper (and slower) implementation.
        if (clump_indices[robot.pos] == -1) {
            for (const auto &dir : DIRECTIONS) {
                Pos neighbor = robot.pos + aoc::Delta(dir, true);
                if (robot_counts.in_bounds(neighbor) &&
                    robot_counts[neighbor] > 0) {
                    if (clump_indices[neighbor] != -1) {
                        clump_indices[robot.pos] = clump_indices[neighbor];
                        ++clumps[clump_indices[robot.pos]];
                        break;
                    }
                }
            }
            if (clump_indices[robot.pos] == -1) {
                // new clump
                clump_indices[robot.pos] = clumps.size();
                clumps.push_back(1);
            }
        }
    }
    return *std::max_element(clumps.begin(), clumps.end());
}

std::ostream &operator<<(std::ostream &os, const Robots &robots) {
    return robots.robot_counts.custom_print(os, [&os](auto count) {
        if (count == 0) {
            os << ' ';
        } else if (count < 10) {
            os << count;
        } else {
            os << '*';
        }
    });
}

} // namespace aoc::day14

#endif /* end of include guard: DAY14_HPP_5W9MWUPG */
