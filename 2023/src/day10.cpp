/******************************************************************************
 * File:        day10.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-10
 *****************************************************************************/

// sed 's/|/│/g; s/-/─/g; s/L/└/g; s/J/┘/g; s/7/┐/g; s/F/┌/g; s/\./ /g'

#include "day10.hpp"
#include "lib.hpp"  // for parse_args, Pos, DEBUG
#include <cstdlib>  // for abs
#include <fstream>  // for ifstream
#include <iomanip>  // for setw
#include <iostream> // for cout, cerr

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv).infile;

    using namespace aoc::day10;
    using aoc::Pos;
    PipeGrid grid = read_pipes(infile);

    int perimeter = 0;
    int twice_area = 0;
    PipeIterator it = grid.pipe_iterator();
    do {
        Pos prev_pos = *it;
        ++it;
        ++perimeter;
        Pos curr_pos = *it;
        // shoelace formula for area of an arbitrary polygon
        twice_area += (prev_pos.y + curr_pos.y) * (prev_pos.x - curr_pos.x);
    } while (*it != grid.start_pos);

    int area = std::abs(twice_area) / 2;

    if constexpr (aoc::DEBUG) {
        std::cerr << "area:      " << std::setw(4) << area << "\n";
        std::cerr << "perimeter: " << std::setw(4) << perimeter << "\n";
    }

    int part_1 = perimeter / 2;
    // Pick's theorem: A = i + b/2 - 1
    //                 i = A - b/2 + 1, where
    // * A is the exact area (accounting for partial grid squares)
    // * i is the number of grid points on the interior of the curve
    // * b is the number of grid points on the boundary of the curve
    int part_2 = area - part_1 + 1;

    std::cout << part_1 << "\n";
    std::cout << part_2 << "\n";

    return 0;
}
