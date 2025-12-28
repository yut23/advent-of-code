/******************************************************************************
 * File:        day06.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-12-06
 *****************************************************************************/

#ifndef DAY06_HPP_FLCUWNB1
#define DAY06_HPP_FLCUWNB1

#include "lib.hpp"
#include "unit_test/pretty_print.hpp"
#include <algorithm> // for transform
#include <cstddef>   // for size_t
#include <iostream>  // for istream, cerr
#include <iterator>  // for back_inserter
#include <numeric>   // for reduce
#include <sstream>   // for istringstream
#include <string>    // for string, getline
#include <utility>   // for pair
#include <vector>    // for vector

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
        if constexpr (aoc::DEBUG) {
            std::cerr << static_cast<char>(op) << " "
                      << pretty_print::repr(inputs) << "\n";
        }
        return std::reduce(inputs.begin(), inputs.end(), oper.init(), oper);
    }
};

std::pair<std::vector<MathProblem>, std::vector<MathProblem>>
read_input(std::istream &is) {
    // parse problems for part 1
    std::string line;
    std::vector<std::vector<long>> inputs;
    std::vector<char> ops;
    std::vector<std::string> lines;
    while (std::getline(is, line)) {
        char first = line[line.find_first_not_of(' ')];
        std::istringstream iss(line);
        if (first == '+' || first == '*') {
            ops = aoc::read_vector<char>(iss);
        } else {
            inputs.push_back(aoc::read_vector<long>(iss));
        }
        lines.push_back(std::move(line));
    }
    std::vector<MathProblem> problems1, problems2;
    for (std::size_t i = 0; i < inputs[0].size(); ++i) {
        std::vector<long> values;
        std::transform(inputs.begin(), inputs.end(), std::back_inserter(values),
                       [i](const auto &row) { return row[i]; });
        problems1.push_back({values, Operation(ops[i])});
    }

    // parse problems for part 2
    std::vector<std::string> transposed_lines(lines[0].size(),
                                              std::string(lines.size(), ' '));
    for (std::size_t i = 0; i < lines.size(); ++i) {
        for (std::size_t j = 0; j < lines[0].size(); ++j) {
            transposed_lines.at(j).at(i) = lines.at(i).at(j);
        }
    }
    MathProblem problem{};
    for (std::size_t i = 0; i < transposed_lines.size(); ++i) {
        line = transposed_lines[i];
        if (line.find_first_not_of(' ') == std::string::npos) {
            // blank line
            problems2.push_back(std::move(problem));
            problem = {};
            continue;
        }
        problem.inputs.push_back(std::stol(line));
        char last = line.back();
        if (last == '+' || last == '*') {
            problem.op = Operation(last);
        }
    }
    if (!problem.inputs.empty()) {
        // save last problem
        problems2.push_back(std::move(problem));
    }
    return {problems1, problems2};
}

} // namespace aoc::day06

#endif /* end of include guard: DAY06_HPP_FLCUWNB1 */
