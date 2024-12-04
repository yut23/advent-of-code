/******************************************************************************
 * File:        day06.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-06
 *****************************************************************************/

#include "lib.hpp"
#include <cmath>    // for sqrt
#include <cstddef>  // for size_t
#include <fstream>  // for ifstream
#include <iomanip>  // for setprecision
#include <iostream> // for cout
#include <sstream>  // for istringstream, stringstream
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

long count_wins(long time, long distance) {
    // get the lower and upper bounds by solving:
    //  (time - x) * x - distance = 0
    //    x^2 - time*x + distance = 0
    // quadratic formula: (-b ± sqrt(b*b - 4*a*c)) / (2*a)
    double determinant = std::sqrt(time * time - 4 * distance);
    double lo = (time - determinant) / 2;
    double hi = (time + determinant) / 2;
    // upper bound is exclusive
    if (std::trunc(hi) == hi) {
        hi -= 1;
    }
    long count = static_cast<long>(hi) - static_cast<long>(lo);
    if constexpr (aoc::DEBUG) {
        std::cerr << count << " (" << std::setprecision(17) << hi << " - " << lo
                  << ")\n";
    }
    return count;
}

long join_numbers(const std::vector<int> &vec) {
    std::stringstream ss;
    for (int x : vec) {
        ss << x;
    }
    long result;
    ss >> result;
    return result;
}

long part_1(const std::vector<int> &times, const std::vector<int> &records) {
    long product = 1;
    for (std::size_t i = 0; i < times.size(); ++i) {
        product *= count_wins(times[i], records[i]);
    }
    return product;
}

long part_2(const std::vector<int> &times, const std::vector<int> &records) {
    // join all the numbers together
    long time = join_numbers(times);
    long distance = join_numbers(records);
    return count_wins(time, distance);
}
} // namespace aoc::day06

int main(int argc, char **argv) {
    std::vector<int> times, records;
    {
        std::ifstream infile = aoc::parse_args(argc, argv);
        aoc::day06::parse(infile, times, records);
    }

    std::cout << aoc::day06::part_1(times, records) << "\n";
    std::cout << aoc::day06::part_2(times, records) << "\n";

    return 0;
}
