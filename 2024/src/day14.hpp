/******************************************************************************
 * File:        day14.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-14
 *****************************************************************************/

#ifndef DAY14_HPP_5W9MWUPG
#define DAY14_HPP_5W9MWUPG

#include "lib.hpp"  // for Pos, Delta, expect_input
#include <array>    // for array
#include <iostream> // for istream
#include <string>   // for string, getline
#include <utility>  // for move
#include <vector>   // for vector

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

auto read_input(std::istream &is) {
    Robot robot;
    std::vector<Robot> robots;
    while (is >> robot) {
        robots.push_back(std::move(robot));
    }
    return robots;
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

int safety_factor(const std::vector<Robot> &robots, const Pos &bounds) {
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

} // namespace aoc::day14

#endif /* end of include guard: DAY14_HPP_5W9MWUPG */
