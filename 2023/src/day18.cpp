/******************************************************************************
 * File:        day18.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-18
 *****************************************************************************/

#include "day18.hpp"
#include "lib.hpp"  // for parse_args, Pos, Delta
#include <cstdlib>  // for abs
#include <iostream> // for cout

int part_1(const std::vector<aoc::day18::DigInstruction> &instructions) {
    using namespace aoc;
    // same as day 10, shoelace formula and Pick's theorem
    int perimeter = 0;
    int twice_area = 0;
    Pos curr_pos(0, 0);
    for (const auto &instr : instructions) {
        Pos prev_pos = curr_pos;
        curr_pos += Delta(instr.dir) * instr.length;
        perimeter += instr.length;
        twice_area += (prev_pos.y + curr_pos.y) * (prev_pos.x - curr_pos.x);
    }
    int area = std::abs(twice_area) / 2;

    // Pick's theorem: A = i + b/2 - 1
    int interior = area - perimeter / 2 + 1;
    return perimeter + interior;
}

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    std::vector<aoc::day18::DigInstruction> instructions =
        aoc::day18::read_instructions(infile);

    std::cout << part_1(instructions) << "\n";

    return 0;
}
