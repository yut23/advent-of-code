/******************************************************************************
 * File:        day09.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-12-09
 *****************************************************************************/

#include "day09.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout
#include <vector>   // for vector, max

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv).infile;

    auto points = aoc::day09::read_input(infile);

    long max_area = 0;
    for (std::size_t i = 0; i < points.size(); ++i) {
        auto p_i = points[i];
        for (std::size_t j = i + 1; j < points.size(); ++j) {
            max_area = std::max(max_area, aoc::day09::area(p_i, points[j]));
        }
    }
    std::cout << max_area << "\n";

    return 0;
}
