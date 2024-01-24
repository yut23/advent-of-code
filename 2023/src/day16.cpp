/******************************************************************************
 * File:        day16.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-17
 *****************************************************************************/

#include "day16.hpp"
#include "lib.hpp"          // for parse_args, Pos, AbsDirection, DEBUG
#include <initializer_list> // for initializer_list
#include <iostream>         // for cout, cerr

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    auto laser_grid = aoc::day16::LaserGrid::read(infile);

    if constexpr (aoc::DEBUG) {
        std::cerr << laser_grid << "\n";
    }

    // TODO: optimize this
    // some sort of DP should work, but loops will need special handling

    int max_energized = 0;
    aoc::Pos pos(0, 0);
    for (pos.y = 0; pos.y < laser_grid.height; ++pos.y) {
        for (auto dir : {aoc::AbsDirection::east, aoc::AbsDirection::west}) {
            pos.x = dir == aoc::AbsDirection::east ? 0 : laser_grid.width - 1;
            laser_grid.send_beam(pos, dir);
            int count = laser_grid.count_energized();
            if (pos.x == 0 && pos.y == 0) {
                // part 1
                if constexpr (aoc::DEBUG) {
                    laser_grid.print_energized(std::cerr);
                    std::cerr << "\n";
                }
                std::cout << count << "\n";
            }
            if (max_energized < count) {
                max_energized = count;
            }
            laser_grid.clear_energized();
        }
    }

    for (pos.x = 0; pos.x < laser_grid.width; ++pos.x) {
        for (auto dir : {aoc::AbsDirection::south, aoc::AbsDirection::north}) {
            pos.y = dir == aoc::AbsDirection::south ? 0 : laser_grid.height - 1;
            laser_grid.send_beam(pos, dir);
            int count = laser_grid.count_energized();
            if (max_energized < count) {
                max_energized = count;
            }
            laser_grid.clear_energized();
        }
    }

    std::cout << max_energized << "\n";

    return 0;
}
