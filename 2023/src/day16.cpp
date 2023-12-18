/******************************************************************************
 * File:        day16.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-17
 *****************************************************************************/

#include "day16.hpp"
#include "lib.hpp"  // for parse_args, DEBUG
#include <iostream> // for cout, cerr

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    auto grid = aoc::day16::Grid::read(infile);

    if constexpr (aoc::DEBUG) {
        std::cerr << grid << "\n";
    }

    // TODO: optimize this
    // some sort of DP should work, but loops will need special handling

    int max_energized = 0;
    aoc::Pos pos(0, 0);
    for (pos.y = 0; pos.y < grid.height(); ++pos.y) {
        for (pos.x = 0; pos.x < grid.width(); pos.x += grid.width() - 1) {
            aoc::AbsDirection dir =
                pos.x == 0 ? aoc::AbsDirection::east : aoc::AbsDirection::west;
            grid.send_beam(pos, dir);
            int count = grid.count_energized();
            if (pos.x == 0 && pos.y == 0) {
                // part 1
                if constexpr (aoc::DEBUG) {
                    grid.print_energized(std::cerr);
                    std::cerr << "\n";
                }
                std::cout << count << "\n";
            }
            if (max_energized < count) {
                max_energized = count;
            }
            grid.clear_energized();
        }
    }

    for (pos.x = 0; pos.x < grid.width(); ++pos.x) {
        for (pos.y = 0; pos.y < grid.height(); pos.y += grid.height() - 1) {
            aoc::AbsDirection dir = pos.y == 0 ? aoc::AbsDirection::south
                                               : aoc::AbsDirection::north;
            grid.send_beam(pos, dir);
            int count = grid.count_energized();
            if (max_energized < count) {
                max_energized = count;
            }
            grid.clear_energized();
        }
    }

    std::cout << max_energized << "\n";

    return 0;
}
