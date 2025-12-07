/******************************************************************************
 * File:        day02.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-12-02
 *****************************************************************************/

#include "day02.hpp"
#include "lib.hpp"
#include "unit_test/pretty_print.hpp" // for repr
#include "util/util.hpp"              // for constexpr_for
#include <fstream>                    // for ifstream
#include <iostream>                   // for cout
#include <numeric>                    // for reduce

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv).infile;

    using namespace aoc::day02;
    Range range{};
    unsigned long part1 = 0, part2 = 0;
    while (infile >> range) {
        if constexpr (aoc::DEBUG)
            std::cerr << "range " << range << ": ";
        std::set<int_t> ids = find_invalid_ids<2>(range);
        part1 += std::reduce(ids.begin(), ids.end());
        util::constexpr_for<3, 11>(
            [&](auto REPEATS) { ids.merge(find_invalid_ids<REPEATS>(range)); });
        part2 += std::reduce(ids.begin(), ids.end());
        if constexpr (aoc::DEBUG)
            std::cerr << pretty_print::repr(ids) << "\n";
    }
    std::cout << part1 << "\n";
    std::cout << part2 << "\n";

    return 0;
}
