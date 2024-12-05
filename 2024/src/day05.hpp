/******************************************************************************
 * File:        day05.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-05
 *****************************************************************************/

#ifndef DAY05_HPP_JWAIHBOR
#define DAY05_HPP_JWAIHBOR

#include "lib.hpp" // for expect_input
#if DEBUG_MODE
#include "unit_test/pretty_print.hpp" // for repr
#endif
#include <algorithm> // for ranges::set_difference
#include <iostream>  // for istream
#include <iterator>  // for back_inserter
#include <map>       // for map
#include <set>       // for set
#include <sstream>   // for istringstream
#include <string>    // for string, getline
#include <utility>   // for move, pair
#include <vector>    // for vector
// IWYU pragma: no_include <functional>  // for identity, less (ranges::set_difference)

namespace aoc::day05 {

class Rules {
    // mapping from page p to set of pages that shouldn't occur before p
    std::map<int, std::set<int>> anti_deps;

    Rules() = default;

  public:
    static std::pair<Rules, std::vector<std::vector<int>>> read(std::istream &);

    bool is_valid_ordering(const std::vector<int> &pages) const;
};

std::pair<Rules, std::vector<std::vector<int>>> Rules::read(std::istream &is) {
    Rules rules;
    std::string line;
    int first, second;
    while (std::getline(is, line) && !line.empty()) {
        std::istringstream iss{line};
        if (iss >> first >> expect_input('|') >> second) {
            rules.anti_deps[first].insert(second);
        }
    }
#if DEBUG_MODE
    std::cerr << "anti-dependencies:\n";
    for (const auto &[p, dep] : rules.anti_deps) {
        std::cerr << "  " << p << ": " << pretty_print::repr(dep) << "\n";
    }
    std::cerr << "\n";
#endif

    std::vector<std::vector<int>> orderings;
    std::vector<int> pages;
    while (std::getline(is, line)) {
        // split on commas
        std::istringstream iss{line};
        int page = 0;
        if (iss >> page) {
            pages.push_back(page);
        }
        while (iss >> expect_input(',') >> page) {
            pages.push_back(page);
        }
        orderings.push_back(std::move(pages));
    }
    return {rules, orderings};
}

bool Rules::is_valid_ordering(const std::vector<int> &pages) const {
#if DEBUG_MODE
    using pretty_print::repr;
    int i = 0;
#endif
    std::set<int> seen{};
    for (const int p : pages) {
        std::vector<int> invalid;
        auto it = anti_deps.find(p);
#if DEBUG_MODE
        std::cerr << "page " << i << " = " << p << ": seen=" << repr(seen);
#endif
        if (it != anti_deps.end()) {
            std::ranges::set_intersection(it->second, seen,
                                          std::back_inserter(invalid));
#if DEBUG_MODE
            std::cerr << ", anti_deps=" << repr(it->second) << "\n";
#endif
            if (!invalid.empty()) {
#if DEBUG_MODE
                std::cerr << "  seen shouldn't contain " << repr(invalid)
                          << "\n";
#endif
                return false;
            }
        } else {
#if DEBUG_MODE
            std::cerr << ", anti_deps={}\n";
#endif
        }
        seen.insert(p);
#if DEBUG_MODE
        ++i;
#endif
    }
    return true;
}

} // namespace aoc::day05

#endif /* end of include guard: DAY05_HPP_JWAIHBOR */
