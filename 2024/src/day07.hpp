/******************************************************************************
 * File:        day07.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-07
 *****************************************************************************/

#ifndef DAY07_HPP_BVIUF38H
#define DAY07_HPP_BVIUF38H

#include "lib.hpp"       // for expect_input
#include "util/math.hpp" // for next_power_of_10
#include <cstddef>       // for size_t
#include <iostream>      // for istream
#include <sstream>       // for istringstream
#include <string>        // for string, getline
#include <utility>       // for move
#include <vector>        // for vector

namespace aoc::day07 {

struct Equation {
    long test_value;
    std::vector<long> operands;

  private:
    bool is_valid_helper(bool use_concat, long target, std::size_t idx) const;

  public:
    bool is_valid(bool use_concat) const {
        return is_valid_helper(use_concat, test_value, operands.size() - 1);
    }
};

bool Equation::is_valid_helper(bool use_concat, long target,
                               std::size_t idx) const {
    // work backwards from the test value
    if (idx == 0) {
        return target == operands[idx];
    }
    long operand = operands[idx];
    if (operand > target) {
        // all operands make the number larger
        return false;
    }
    // multiplication
    if (target % operand == 0 &&
        is_valid_helper(use_concat, target / operand, idx - 1)) {
        return true;
    }
    // concatenation
    if (use_concat) {
        long pow10 = math::next_power_of_10(operand);
        if (target % pow10 == operand &&
            is_valid_helper(use_concat, target / pow10, idx - 1)) {
            return true;
        }
    }
    // addition
    return is_valid_helper(use_concat, target - operand, idx - 1);
}

std::istream &operator>>(std::istream &is, Equation &eqn) {
    std::string line;
    long test_value;
    std::vector<long> operands;
    if (std::getline(is, line)) {
        std::istringstream iss{line};
        iss >> test_value >> expect_input(':');
        long x;
        while (iss >> x) {
            operands.push_back(x);
        }
    }
    if (is) {
        eqn.test_value = test_value;
        eqn.operands = std::move(operands);
    }
    return is;
}

auto read_input(std::istream &is) {
    std::vector<Equation> equations;
    Equation eqn;
    while (is >> eqn) {
        equations.push_back(std::move(eqn));
    }
    return equations;
}

} // namespace aoc::day07

#endif /* end of include guard: DAY07_HPP_BVIUF38H */
