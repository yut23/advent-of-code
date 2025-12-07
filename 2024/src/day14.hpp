/******************************************************************************
 * File:        day14.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-14
 *****************************************************************************/

#ifndef DAY14_HPP_5W9MWUPG
#define DAY14_HPP_5W9MWUPG

#include "ds/grid.hpp" // for Grid
#include "lib.hpp"     // for Pos, Delta, expect_input
#include <array>       // for array
#include <cmath>       // for log
#include <cstddef>     // for size_t
#include <iostream>    // for istream, ostream, noskipws, ws
#include <utility>     // for move, pair
#include <vector>      // for vector

namespace aoc::day14 {

struct Robot {
    Pos pos{0, 0};
    Delta vel{0, 0};
    std::size_t grid_index{0};

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

struct Robots {
    std::vector<Robot> robots;
    Pos bounds;

    aoc::ds::Grid<int> robot_counts;

    explicit Robots(const Pos &bounds)
        : robots(), bounds(bounds), robot_counts(bounds.x, bounds.y, 0) {}

  public:
    static Robots read(std::istream &is, const Pos &bounds);
    void update();
    int safety_factor() const;
    std::pair<double, double> calc_entropy() const;

    friend std::ostream &operator<<(std::ostream &os, const Robots &robots);
};

Robots Robots::read(std::istream &is, const Pos &bounds) {
    Robot robot;
    Robots robots(bounds);
    while (is >> robot) {
        robot.grid_index = robots.robot_counts.get_index(robot.pos);
        ++robots.robot_counts[robot.grid_index];
        robots.robots.push_back(std::move(robot));
    }
    return robots;
}

void Robots::update() {
    for (auto &robot : robots) {
        --robot_counts[robot.grid_index];
        robot.update(bounds);
        robot.grid_index = robot_counts.get_index(robot.pos);
        ++robot_counts[robot.grid_index];
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

/**
 * Calculates the Shannon entropy of the x-coordinates and y-coordinates of the
 * robot positions.
 */
std::pair<double, double> Robots::calc_entropy() const {
    // coordinates can take on values in [0, bounds)
    std::vector<std::size_t> x_counts(bounds.x, 0);
    std::vector<std::size_t> y_counts(bounds.y, 0);
    for (const auto &robot : robots) {
        ++x_counts[robot.pos.x];
        ++y_counts[robot.pos.y];
    }
    // calculate entropy as \sum -p log p
    double x_entropy = 0, y_entropy = 0;
    for (const std::size_t count : x_counts) {
        if (count > 0) {
            double p = static_cast<double>(count) / robots.size();
            x_entropy -= p * std::log(p);
        }
    }
    for (const std::size_t count : y_counts) {
        if (count > 0) {
            double p = static_cast<double>(count) / robots.size();
            y_entropy -= p * std::log(p);
        }
    }
    return {x_entropy, y_entropy};
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
