/******************************************************************************
 * File:        day05.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-05
 *****************************************************************************/

#include "day05.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout

int main(int argc, char **argv) {
#if DEBUG_MODE
    using pretty_print::repr;
#endif
    std::ifstream infile = aoc::parse_args(argc, argv);

    auto [rules, orderings] = aoc::day05::Rules::read(infile);

    int total = 0;
    for (const auto &pages : orderings) {
        if (rules.is_valid_ordering(pages)) {
#if DEBUG_MODE
            std::cerr << "valid: " << repr(pages) << "\n\n";
#endif
            total += pages[pages.size() / 2];
        } else {
#if DEBUG_MODE
            std::cerr << "not valid: " << repr(pages) << "\n\n";
#endif
        }
    }

    std::cout << total << "\n";

    return 0;
}
