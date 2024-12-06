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
#include <algorithm> // for ranges::set_difference, ranges::set_intersection, ranges::sort
#include <functional> // for ranges::greater
#include <iostream>   // for istream
#include <iterator>   // for back_inserter
#include <map>        // for map
#include <set>        // for set
#include <sstream>    // for istringstream
#include <string>     // for string, getline
#include <utility>    // for move, pair
#include <vector>     // for vector

namespace aoc::day05 {

#if DEBUG_MODE
using pretty_print::repr;
#endif

class Rules {
    // mapping from page p to set of pages that may only occur after p
    std::map<int, std::set<int>> successors;

    Rules() = default;
    std::vector<int> fix_ordering(const std::vector<int> &pages) const;

    std::vector<int> get_successors(const std::vector<int> &pages, int p) const;
    std::vector<int> get_successors(std::vector<int> pages,
                                    const std::set<int> &all_successors) const;

  public:
    static std::pair<Rules, std::vector<std::vector<int>>> read(std::istream &);

    // returns an empty vector if the ordering is valid, or the fixed ordering
    // if invalid
    std::vector<int> check_ordering(const std::vector<int> &pages) const;
};

std::pair<Rules, std::vector<std::vector<int>>> Rules::read(std::istream &is) {
    Rules rules;
    std::string line;
    int first, second;
    while (std::getline(is, line) && !line.empty()) {
        std::istringstream iss{line};
        if (iss >> first >> expect_input('|') >> second) {
            rules.successors[first].insert(second);
        }
    }
#if DEBUG_MODE
    std::cerr << "successors:\n";
    for (const auto &[p, dep] : rules.successors) {
        std::cerr << "  " << p << ": " << repr(dep) << "\n";
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

std::vector<int> Rules::get_successors(const std::vector<int> &pages,
                                       int p) const {
    auto it = successors.find(p);
    if (it != successors.end()) {
        return get_successors(pages, it->second);
    }
    return {};
}

std::vector<int>
Rules::get_successors(std::vector<int> pages,
                      const std::set<int> &all_successors) const {
    std::ranges::sort(pages);
    std::vector<int> succ{};
    std::ranges::set_intersection(all_successors, pages,
                                  std::back_inserter(succ));
    return succ;
}

std::vector<int> Rules::check_ordering(const std::vector<int> &pages) const {
    std::set<int> seen{};
    std::vector<int> invalid{};
#if DEBUG_MODE
    int i = 0;
#endif
    for (const int p : pages) {
        auto it = successors.find(p);
#if DEBUG_MODE
        std::cerr << "page " << i << " = " << p << ": seen=" << repr(seen);
#endif
        if (it != successors.end()) {
            std::ranges::set_intersection(it->second, seen,
                                          std::back_inserter(invalid));
#if DEBUG_MODE
            std::cerr << ", successors="
                      << repr(get_successors(pages, it->second)) << "\n";
#endif
            if (!invalid.empty()) {
#if DEBUG_MODE
                std::cerr << "  seen shouldn't contain " << repr(invalid)
                          << "\n";
#endif
                return fix_ordering(pages);
            }
        } else {
#if DEBUG_MODE
            std::cerr << ", no successors\n";
#endif
        }
        seen.insert(p);
#if DEBUG_MODE
        ++i;
#endif
    }
    // this is empty
    return invalid;
}

std::vector<int> Rules::fix_ordering(const std::vector<int> &pages) const {
#if DEBUG_MODE
    std::cerr << "filtered successors:\n";
    for (int p : pages) {
        std::cerr << "  " << p << ": " << repr(get_successors(pages, p))
                  << "\n";
    }
#endif

    // note: this assumes that the filtered list of successors is
    // exhaustive (i.e. all transitive successors are already included),
    // and each topological generation contains a single page (i.e. no
    // ambiguity)
    std::vector<int> new_ordering{pages};
    std::ranges::sort(
        new_ordering, std::ranges::greater{},
        [this, &pages](int p) { return get_successors(pages, p).size(); });
#if DEBUG_MODE
    std::cerr << "new ordering: " << repr(new_ordering) << "\n";
#endif

    return new_ordering;
}

} // namespace aoc::day05

#endif /* end of include guard: DAY05_HPP_JWAIHBOR */
