/******************************************************************************
 * File:        day04.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-04
 *****************************************************************************/

#include "lib.hpp"
#include <iostream> // for cout
#include <set>      // for set
#include <sstream>  // for istringstream
#include <string>   // for string, getline

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    // read file line-by-line
    std::string line;
    int part_1 = 0;
    while (std::getline(infile, line)) {
        std::istringstream ss{line};
        ss >> aoc::skip(2);
        std::string number;
        std::set<std::string> winning_numbers;
        while (ss >> number) {
            if (number == "|") {
                break;
            }
            winning_numbers.emplace(number);
        }
        int matches = 0;
        while (ss >> number) {
            if (winning_numbers.contains(number)) {
                ++matches;
            }
        }
        if (matches > 0) {
            part_1 += (1 << (matches - 1));
        }
    }
    std::cout << part_1 << "\n";
    return 0;
}
