/******************************************************************************
 * File:        day12.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-12-12
 *****************************************************************************/

#ifndef DAY12_HPP_Q1XGZIYD
#define DAY12_HPP_Q1XGZIYD

#include "lib.hpp"   // for expect_input
#include <algorithm> // for count
#include <cassert>   // for assert
#include <iostream>  // for istream, ostream
#include <numeric>   // for inner_product, reduce
#include <string>    // for string, getline
#include <utility>   // for move, pair
#include <vector>    // for vector

namespace aoc::day12 {

struct Region {
    int width, length;
    std::vector<int> presents;

    int area_3x3() const { return (width / 3) * (length / 3) * 9; }
    // minimum possible area in which the presents could fit, assuming they
    // tile perfectly with no empty space
    int present_area_lower_bound(const std::vector<int> &present_sizes) const {
        assert(presents.size() == present_sizes.size());
        return std::inner_product(presents.begin(), presents.end(),
                                  present_sizes.begin(), 0);
    }
    // maximum possible area required, by giving each present its own 3x3 block
    int present_area_upper_bound() const {
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

std::pair<std::vector<Region>, std::vector<int>> read_input(std::istream &is) {
    std::vector<Region> regions;
    std::vector<int> present_sizes;
    Region region;
    int num_presents = 0;
    int x;
    char c;
    while (is >> region.width >> c) {
        if (c == ':') {
            ++num_presents;
            // skip the first line
            std::string line;
            std::getline(is, line);
            // count the number of non-empty squares
            int present_size = 0;
            int height = 0;
            while (true) {
                std::getline(is, line);
                if (line.empty()) {
                    break;
                }
                present_size += std::count(line.begin(), line.end(), '#');
                // presents should be 3 units wide, plus a newline
                assert(line.size() == 3);
                ++height;
            }
            // presents should be 3 units tall
            assert(height == 3);
            present_sizes.push_back(present_size);
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
    return {regions, present_sizes};
}

} // namespace aoc::day12

#endif /* end of include guard: DAY12_HPP_Q1XGZIYD */
