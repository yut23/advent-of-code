/******************************************************************************
 * File:        day09.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-09
 *****************************************************************************/

#include "day09.hpp"
#include "lib.hpp"   // for parse_args
#include <algorithm> // for any_of
#include <iostream>  // for cout
#include <stack>     // for stack
#include <string>    // for string, getline
#include <vector>    // for vector

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    // read file line-by-line
    std::string line;
    int part_1 = 0, part_2 = 0;
    while (std::getline(infile, line)) {
        using namespace aoc::day09;
        std::stack<std::vector<int>> diffs;
        diffs.push(read_history(line));
        while (std::ranges::any_of(diffs.top(),
                                   [](const auto &x) { return x != 0; })) {
            diffs.push(diff(diffs.top()));
        }
        diffs.pop();
        int next = 0, prev = 0;
        while (!diffs.empty()) {
            next = diffs.top().back() + next;
            prev = diffs.top().front() - prev;
            diffs.pop();
        }
        part_1 += next;
        part_2 += prev;
    }

    std::cout << part_1 << "\n";
    std::cout << part_2 << "\n";

    return 0;
}
