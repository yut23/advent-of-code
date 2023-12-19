/******************************************************************************
 * File:        day18.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-18
 *****************************************************************************/

#ifndef DAY18_HPP_MRGUXK1T
#define DAY18_HPP_MRGUXK1T

#include "lib.hpp"  // for AbsDirection
#include <iostream> // for istream
#include <string>   // for string
#include <utility>  // for move
#include <vector>   // for vector

namespace aoc::day18 {

struct DigInstruction {
    AbsDirection dir;
    int length;
    std::string color;
};

std::istream &operator>>(std::istream &is, DigInstruction &instr) {
    DigInstruction tmp;
    if (is >> tmp.dir >> tmp.length >> tmp.color) {
        instr = std::move(tmp);
    }
    return is;
}

std::vector<DigInstruction> read_instructions(std::istream &is) {
    std::vector<DigInstruction> instructions;
    DigInstruction instr{};
    while (is >> instr) {
        instructions.push_back(std::move(instr));
    }
    return instructions;
}

} // namespace aoc::day18

#endif /* end of include guard: DAY18_HPP_MRGUXK1T */
