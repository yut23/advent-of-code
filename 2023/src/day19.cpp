/******************************************************************************
 * File:        day19.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-19
 *****************************************************************************/

#include "day19.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout
#include <numeric>  // for reduce
#include <vector>   // for vector

int part_1(const aoc::day19::PartCategorizer &cat,
           const std::vector<aoc::day19::Part> &parts) {
    int sum = 0;
    for (const auto &part : parts) {
        std::string curr = "in";
        if constexpr (aoc::DEBUG) {
            std::cerr << "processing part {x=" << part[0] << ",m=" << part[1]
                      << ",a=" << part[2] << ",s=" << part[3] << "}: in";
        }
        while (curr != "A" && curr != "R") {
            curr = cat.at(curr).lookup(part);
            if constexpr (aoc::DEBUG) {
                std::cerr << " -> " << curr;
            }
        }
        if constexpr (aoc::DEBUG) {
            std::cerr << "\n";
        }
        if (curr == "A") {
            sum += std::reduce(part.begin(), part.end());
        }
    }
    return sum;
}

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    const auto &[cat, parts] = aoc::day19::read_input(infile);

    std::cout << part_1(cat, parts) << "\n";

    std::cout << aoc::day19::Part2Solver(cat).solve() << "\n";

    return 0;
}
