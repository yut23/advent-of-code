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
#include <set>      // for set
#include <vector>   // for vector, max

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv).infile;

    auto points = aoc::day09::read_input(infile);

    long part_1 = 0;
    // for path compression
    std::set<int> x_values, y_values;
    for (std::size_t i = 0; i < points.size(); ++i) {
        auto p_i = points[i];
        x_values.insert(p_i.x);
        y_values.insert(p_i.y);
        for (std::size_t j = i + 1; j < points.size(); ++j) {
            part_1 = std::max(part_1, aoc::day09::calc_area(p_i, points[j]));
        }
    }

    std::cout << part_1 << "\n";

    long part_2 = aoc::day09::solve_part_2(points, x_values, y_values);
    std::cout << part_2 << "\n";

    return 0;
}
