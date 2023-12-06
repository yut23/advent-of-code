/******************************************************************************
 * File:        day06.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-06
 *****************************************************************************/

#include "lib.hpp"
#include <cstddef>  // for size_t
#include <iostream> // for cout
#include <sstream>  // for istringstream
#include <string>   // for string, getline
#include <vector>   // for vector

namespace aoc::day06 {

void parse(std::istream &is, std::vector<int> &times,
           std::vector<int> &records) {
    std::string line;
    std::getline(is, line);
    std::istringstream time_ss{line};
    std::getline(is, line);
    std::istringstream record_ss{line};
    // skip labels
    time_ss >> aoc::skip(1);
    record_ss >> aoc::skip(1);
    int time, record;
    while (time_ss >> time && record_ss >> record) {
        times.push_back(time);
        records.push_back(record);
    }
}

int count_wins(int time, int distance) {
    int count = 0;
    for (int n = 0; n < time; ++n) {
        if ((time - n) * n > distance) {
            ++count;
        }
    }
    return count;
}

} // namespace aoc::day06

int main(int argc, char **argv) {
    std::vector<int> times, records;
    {
        std::ifstream infile = aoc::parse_args(argc, argv);
        aoc::day06::parse(infile, times, records);
    }

    int part_1 = 1;
    for (std::size_t i = 0; i < times.size(); ++i) {
        part_1 *= aoc::day06::count_wins(times[i], records[i]);
    }

    std::cout << part_1 << "\n";

    return 0;
}
