/******************************************************************************
 * File:        day01.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-01
 *****************************************************************************/

#include "lib.hpp"   // for parse_args
#include <algorithm> // for find
#include <fstream>   // for ifstream
#include <iostream>  // for cout
#include <iterator>  // for distance
#include <regex>     // for regex, sregex_iterator
#include <string>    // for string, getline
#include <vector>    // for vector

const std::vector<std::string> digit_names{
    "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};

int parse_digit(std::string match, const bool reverse = false) {
    if (match.length() == 1) {
        return match[0] - '0';
    } else {
        if (reverse) {
            match = std::string{match.rbegin(), match.rend()};
        }
        return std::distance(
                   digit_names.begin(),
                   std::find(digit_names.begin(), digit_names.end(), match)) +
               1;
    }
}

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    // part 1
    const std::string digits{"123456789"};

    // read file line-by-line
    std::string line;
    int total = 0;
    while (std::getline(infile, line)) {
        int first_digit = line[line.find_first_of(digits)] - '0';
        int last_digit = line[line.find_last_of(digits)] - '0';
        total += 10 * first_digit + last_digit;
    }

    std::cout << total << "\n";

    // part 2
    // reset infile
    infile.clear();
    infile.seekg(0, std::ios::beg);

    const std::regex digit_regex{
        "[1-9]|one|two|three|four|five|six|seven|eight|nine"};
    const std::regex rev_digit_regex{
        "[1-9]|enin|thgie|neves|xis|evif|ruof|eerht|owt|eno"};

    total = 0;
    while (std::getline(infile, line)) {
        auto digit_iter =
            std::sregex_iterator(line.cbegin(), line.cend(), digit_regex);
        int first_digit = parse_digit(digit_iter->str());
        std::string rev_line{line.rbegin(), line.rend()};
        auto rev_digit_iter = std::sregex_iterator(
            rev_line.cbegin(), rev_line.cend(), rev_digit_regex);
        int last_digit = parse_digit(rev_digit_iter->str(), true);
        total += 10 * first_digit + last_digit;
    }

    std::cout << total << "\n";

    return 0;
}
