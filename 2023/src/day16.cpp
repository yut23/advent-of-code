/******************************************************************************
 * File:        day16.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-17
 *****************************************************************************/

#include "day16.hpp"
#include "lib.hpp"  // for parse_args, Pos, AbsDirection, DEBUG
#include <fstream>  // for ifstream
#include <iostream> // for cout, cerr

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    auto laser_grid = aoc::day16::LaserGrid::read(infile);

    if constexpr (aoc::DEBUG) {
        std::cerr << laser_grid << "\n";
    }

    aoc::day16::GraphHelper helper(laser_grid);

    if constexpr (aoc::DEBUG) {
        std::cerr << "SCC count:  " << helper.components.size() << "\n";
        for (auto i = 0u; i < helper.components.size(); ++i) {
            const auto &component = helper.components[i];
            if (component.size() > 1) {
                std::cerr << "loop at " << i << ": size=" << component.size()
                          << ", successors="
                          << helper.component_successors[i].size() << "\n";
            }
        }
    }

    int max_energized = 0;
    for (const auto &key : helper.sources) {
        int count = helper.count_energized(key);
        if (key.pos.x == 0 && key.pos.y == 0 &&
            key.dir == aoc::AbsDirection::east) {
            // part 1
            std::cout << count << "\n";
        }
        if (max_energized < count) {
            max_energized = count;
        }
    }

    std::cout << max_energized << "\n";

    return 0;
}
