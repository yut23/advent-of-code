/******************************************************************************
 * File:        day04.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-04
 *****************************************************************************/

#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout
#include <map>      // for map
#include <set>      // for set
#include <sstream>  // for istringstream
#include <string>   // for string, getline

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    // read file line-by-line
    int part_1 = 0, part_2 = 0;
    std::map<int, int> counts{};
    std::string line;
    for (int card_number = 1; std::getline(infile, line); ++card_number) {
        counts[card_number] += 1;
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
        part_2 += counts[card_number];
        for (int i = 1; i <= matches; ++i) {
            counts[card_number + i] += counts[card_number];
        }
    }
    std::cout << part_1 << "\n";
    std::cout << part_2 << "\n";
    return 0;
}
