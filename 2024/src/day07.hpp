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
#include <sstream>       // for istringstream, stringstream
#include <string>        // for string, getline
#include <utility>       // for move
#include <vector>        // for vector

namespace aoc::day07 {

struct Equation {
    long test_value;
    std::vector<long> operands;

  private:
    bool is_valid_helper(bool use_concat, long acc, std::size_t idx) const;

  public:
    bool is_valid(bool use_concat) const {
        return is_valid_helper(use_concat, 0, 0);
    }
};

long concat(long x, long y) { return x * math::next_power_of_10(y) + y; }

bool Equation::is_valid_helper(bool use_concat, long acc,
                               std::size_t idx) const {
    // try all possible operator combinations using recursion
    if (acc > test_value) {
        // all operators make the number larger
        return false;
    }
    if (idx >= operands.size()) {
        // base case: check the value in the accumulator
        return acc == test_value;
    }
    // this will short-circuit if we find a valid operator combination
    bool result = is_valid_helper(use_concat, acc + operands[idx], idx + 1) ||
                  is_valid_helper(use_concat, acc * operands[idx], idx + 1);
    if (!result && use_concat) {
        result =
            is_valid_helper(use_concat, concat(acc, operands[idx]), idx + 1);
    }
    return result;
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
