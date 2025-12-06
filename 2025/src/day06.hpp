/******************************************************************************
 * File:        day06.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-12-06
 *****************************************************************************/

#ifndef DAY06_HPP_FLCUWNB1
#define DAY06_HPP_FLCUWNB1

#include "lib.hpp"
#include <functional> // for multiplies, plus
#include <iostream>   // for istream
#include <numeric>    // for reduce
#include <sstream>    // for istringstream
#include <string>     // for string, getline

namespace aoc::day06 {

enum class Operation : char {
    PLUS = '+',
    TIMES = '*',
};

struct Operator {
    Operation op;

    long init() const { return op == Operation::TIMES; }

    long operator()(long a, long b) const {
        if (op == Operation::PLUS) {
            return a + b;
        } else {
            return a * b;
        }
    }
};

struct MathProblem {
    std::vector<long> inputs;
    Operation op;

    long calculate() const {
        Operator oper{op};
        return std::reduce(inputs.begin(), inputs.end(), oper.init(), oper);
    }
};

auto read_input(std::istream &is) {
    // read file line-by-line
    std::string line;
    std::vector<std::vector<long>> inputs;
    std::vector<char> ops;
    while (std::getline(is, line)) {
        char first = line[line.find_first_not_of(' ')];
        std::istringstream iss(line);
        if (first == '+' || first == '*') {
            ops = aoc::read_vector<char>(iss);
        } else {
            inputs.push_back(aoc::read_vector<long>(iss));
        }
    }
    std::vector<MathProblem> problems;
    for (std::size_t i = 0; i < inputs[0].size(); ++i) {
        std::vector<long> values;
        std::transform(inputs.begin(), inputs.end(), std::back_inserter(values),
                       [i](const auto &row) { return row[i]; });
        problems.push_back({values, Operation(ops[i])});
    }
    return problems;
}

} // namespace aoc::day06

#endif /* end of include guard: DAY06_HPP_FLCUWNB1 */
