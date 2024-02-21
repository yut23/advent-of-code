/******************************************************************************
 * File:        day18.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-18
 *****************************************************************************/

#ifndef DAY18_HPP_MRGUXK1T
#define DAY18_HPP_MRGUXK1T

#include "lib.hpp"  // for AbsDirection
#include <cassert>  // for assert
#include <iostream> // for istream, hex
#include <sstream>  // for istringstream
#include <string>   // for string
#include <utility>  // for move, pair
#include <vector>   // for vector
// IWYU pragma: no_include <algorithm>  // for copy

namespace aoc::day18 {

struct DigInstruction {
    AbsDirection dir;
    int length;
};

std::pair<std::vector<DigInstruction>, std::vector<DigInstruction>>
read_instructions(std::istream &is) {
    std::vector<DigInstruction> instructions1;
    std::vector<DigInstruction> instructions2;
    DigInstruction instr{};
    std::string color;
    while (is >> instr.dir >> instr.length >> color) {
        instructions1.push_back(std::move(instr));
        { // parse color into an instruction
            assert(color.size() == 9);
            assert(color.substr(0, 2) == "(#");
            assert(color[8] == ')');
            assert(color[7] >= '0' && color[7] <= '3');
            DigInstruction color_instr;
            std::istringstream ss{color.substr(2, 5)};
            ss >> std::hex >> color_instr.length;
            // map 0 -> east, 1 -> south, 2 -> west, 3 -> north
            color_instr.dir = AbsDirection(((color[7] - '0') + 1) % 4);
            instructions2.push_back(std::move(color_instr));
        }
    }
    return {std::move(instructions1), std::move(instructions2)};
}

} // namespace aoc::day18

#endif /* end of include guard: DAY18_HPP_MRGUXK1T */
