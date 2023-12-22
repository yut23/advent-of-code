/******************************************************************************
 * File:        day05.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-05
 *****************************************************************************/

#include "day05.hpp"
#include "lib.hpp"   // for parse_args, DEBUG
#include <algorithm> // for min_element, sort
#include <cstddef>   // for size_t
#include <iostream>  // for cerr, cout, ws
#include <limits>    // for numeric_limits
// IWYU pragma: no_include <functional>  // for identity, less (ranges::{min_element,sort})

long part_1(const std::vector<long> &seeds,
            const std::vector<aoc::day05::ConversionMap> &maps) {
    auto results{seeds};
    for (const auto &map : maps) {
        map.apply_in_place(results);
    }
    return *std::ranges::min_element(results);
}

long part_2(const std::vector<long> &seeds,
            const std::vector<aoc::day05::ConversionMap> &maps) {
    /*
     * The insight here is that we only need to keep track of the ranges of
     * values, since applying a map to a range will either shift it (if
     * it's entirely within one MapEntry), or split it into multiple ranges
     * (if it spans several MapEntries).
     */
    using namespace aoc::day05;
    std::vector<Range> seed_ranges;
    for (std::size_t i = 0; i < seeds.size(); i += 2) {
        seed_ranges.emplace_back(seeds[i], seeds[i + 1]);
    }

    for (const auto &map : maps) {
        if constexpr (aoc::DEBUG) {
            std::cerr << "processing " << map.label << " map...\n";
        }
        std::vector<Range> next_ranges;
        for (auto rng : seed_ranges) {
            if constexpr (aoc::DEBUG) {
                std::cerr << "  processing range " << rng << "...\n";
            }
            for (const auto &entry : map.entries) {
                Range new_range = entry.intersect(rng);
                if (new_range.length > 0) {
                    if constexpr (aoc::DEBUG) {
                        std::cerr << "    adding new range " << new_range
                                  << " from " << entry << "\n";
                    }
                    next_ranges.push_back(std::move(new_range));
                }
            }
        }
        seed_ranges = std::move(next_ranges);
    }
    return std::ranges::min_element(seed_ranges)->start;
}

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    // read input
    auto seeds = aoc::day05::read_seeds(infile);
    if constexpr (aoc::DEBUG) {
        std::cerr << "seeds:";
        for (const auto seed : seeds) {
            std::cerr << " " << seed;
        }
        std::cerr << "\n";
    }

    // skip blank line
    infile >> std::ws;

    std::vector<aoc::day05::ConversionMap> maps;
    while (infile) {
        auto map = aoc::day05::ConversionMap::read(infile);
        std::ranges::sort(map.entries);
        // fill in no-op map entries (shift = 0)
        std::size_t N = map.entries.size();
        long curr = 0;
        for (std::size_t i = 0; i < N; ++i) {
            const auto entry = map.entries[i];
            if (curr < entry.start) {
                map.entries.emplace_back(curr, curr, entry.start - curr);
            }
            curr = entry.start + entry.length;
        }
        if (curr < std::numeric_limits<decltype(curr)>::max()) {
            map.entries.emplace_back(
                curr, curr, std::numeric_limits<decltype(curr)>::max() - curr);
        }
        std::ranges::sort(map.entries);
        if constexpr (aoc::DEBUG) {
            std::cerr << "read map: " << map << "\n";
        }
        maps.push_back(std::move(map));
    }

    std::cout << part_1(seeds, maps) << "\n";
    std::cout << part_2(seeds, maps) << "\n";

    // part 2
    return 0;
}
