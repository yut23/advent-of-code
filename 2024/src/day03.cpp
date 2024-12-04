/******************************************************************************
 * File:        day03.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-04
 *****************************************************************************/

#include "day03.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout, cerr
#include <regex>    // for regex, smatch, sregex_iterator
#include <string>   // for string, stoi

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    std::string memory = aoc::day03::read_input(infile);

    std::regex op_regex(R"((mul)\((\d+),(\d+)\)|(do)\(\)|(don't)\(\))");
    std::sregex_iterator op_begin{memory.begin(), memory.end(), op_regex};
    std::sregex_iterator op_end{};

    int total_1 = 0, total_2 = 0;
    bool enabled = true;
    for (auto i = op_begin; i != op_end; ++i) {
        std::smatch match = *i;
        if constexpr (aoc::DEBUG) {
            std::cerr << "found match at " << match.position() << ": "
                      << match.str() << "\n";
        }
        if (match.length(1) > 0) {
            // mul
            int product = std::stoi(match[2].str()) * std::stoi(match[3].str());
            total_1 += product;
            if (enabled) {
                total_2 += product;
            }
        } else if (match.length(4) > 0) {
            // do
            enabled = true;
        } else if (match.length(5) > 0) {
            // don't
            enabled = false;
        }
    }

    std::cout << total_1 << "\n" << total_2 << "\n";

    return 0;
}
