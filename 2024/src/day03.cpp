/******************************************************************************
 * File:        day03.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-04
 *****************************************************************************/

#include "day03.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout
#include <regex>    // for regex, smatch, sregex_iterator
#include <string>   // for string, stoi

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    std::string memory = aoc::day03::read_input(infile);

    // part 1
    int total = 0;

    std::regex mul_regex(R"(mul\((\d+),(\d+)\))");
    std::sregex_iterator mul_begin{memory.begin(), memory.end(), mul_regex};
    std::sregex_iterator mul_end{};
    for (auto i = mul_begin; i != mul_end; ++i) {
        std::smatch match = *i;
        total += std::stoi(match[1].str()) * std::stoi(match[2].str());
    }

    std::cout << total << "\n";

    return 0;
}
