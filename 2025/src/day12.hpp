/******************************************************************************
 * File:        day12.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-12-12
 *****************************************************************************/

#ifndef DAY12_HPP_Q1XGZIYD
#define DAY12_HPP_Q1XGZIYD

#include "lib.hpp"  // for expect_input
#include <iostream> // for istream, ostream
#include <numeric>  // for reduce
#include <string>   // for string, getline
#include <utility>  // for move
#include <vector>   // for vector

namespace aoc::day12 {

struct Region {
    int width, length;
    std::vector<int> presents;

    int area_3x3() const { return (width / 3) * (length / 3) * 9; }
    int present_area_3x3() const {
        return 9 * std::reduce(presents.begin(), presents.end());
    }
};

std::ostream &operator<<(std::ostream &os, const Region &region) {
    os << region.width << 'x' << region.length << ':';
    for (int count : region.presents) {
        os << ' ' << count;
    }
    return os;
}

auto read_input(std::istream &is) {
    std::vector<Region> regions;
    Region region;
    int num_presents = 0;
    int x;
    char c;
    while (is >> region.width >> c) {
        if (c == ':') {
            // ignore the presents for now
            ++num_presents;
            // skip next 4 lines
            for (int i = 0; i < 4; ++i) {
                std::string line;
                std::getline(is, line);
            }
        } else if (c == 'x') {
            is >> region.length >> aoc::expect_input(':');
            for (int i = 0; i < num_presents; ++i) {
                is >> x;
                region.presents.push_back(x);
            }
            regions.push_back(std::move(region));
            region = {};
        }
    }
    return regions;
}

} // namespace aoc::day12

#endif /* end of include guard: DAY12_HPP_Q1XGZIYD */
