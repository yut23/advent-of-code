/******************************************************************************
 * File:        day01.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-01
 *****************************************************************************/

#include "lib.hpp"
#include <iostream> // for cout
#include <numeric>  // for reduce
#include <regex>    // for regex, sregex_iterator
#include <string>   // for string, getline
#include <vector>   // for vector

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    const std::regex digit_regex{"\\d"};

    // read file line-by-line
    std::string line;
    int total = 0;
    while (std::getline(infile, line)) {
        auto digit_iter =
            std::sregex_iterator(line.cbegin(), line.cend(), digit_regex);
        auto digit_end = std::sregex_iterator();
        int first_digit = (digit_iter++)->str()[0] - '0';
        int last_digit = first_digit;
        for (; digit_iter != digit_end; ++digit_iter) {
            last_digit = digit_iter->str()[0] - '0';
        }
        total += 10 * first_digit + last_digit;
    }

    std::cout << total << "\n";
    return 0;
}
