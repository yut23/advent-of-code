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

    int total_1 = 0, total_2 = 0;
    for (const auto &pages : orderings) {
#if DEBUG_MODE
        std::cerr << "pages: " << repr(pages) << "\n";
#endif
        auto new_ordering = rules.check_ordering(pages);
        if (new_ordering.empty()) {
#if DEBUG_MODE
            std::cerr << "valid\n\n";
#endif
            total_1 += pages[pages.size() / 2];
        } else {
#if DEBUG_MODE
            std::cerr << "not valid, new ordering = " << repr(new_ordering)
                      << "\n\n";
#endif
            total_2 += new_ordering[pages.size() / 2];
        }
    }

    std::cout << total_1 << "\n" << total_2 << "\n";

    return 0;
}
