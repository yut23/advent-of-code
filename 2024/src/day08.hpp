/******************************************************************************
 * File:        day08.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-08
 *****************************************************************************/

#ifndef DAY08_HPP_0APLPOEJ
#define DAY08_HPP_0APLPOEJ

#include "lib.hpp"       // for Pos, Delta
#include <iostream>      // for istream
#include <map>           // for map
#include <string>        // for string, getline
#include <unordered_set> // for unordered_set
#include <vector>        // for vector
// IWYU pragma: no_include <functional>  // for hash (unordered_set)
// IWYU pragma: no_include <utility>  // for pair (map)

namespace aoc::day08 {

struct AntennaMap {
    int height;
    int width;
    std::map<char, std::vector<Pos>> antennas{};

    std::unordered_set<Pos> find_antinodes() const;
    bool in_bounds(const Pos &p) const {
        return p.y >= 0 && p.x >= 0 && p.y < height && p.x < width;
    }
};

std::istream &operator>>(std::istream &is, AntennaMap &am) {
    std::string line;
    Pos pos(0, 0);
    for (pos.y = 0; std::getline(is, line); ++pos.y) {
        for (pos.x = 0; pos.x < static_cast<int>(line.size()); ++pos.x) {
            char ch = line[pos.x];
            if (ch != '.') {
                am.antennas[ch].push_back(pos);
            }
        }
    }
    am.height = pos.y;
    am.width = pos.x;
    return is;
}

std::unordered_set<Pos> AntennaMap::find_antinodes() const {
    std::unordered_set<Pos> antinodes;
    const auto add_antinode = [&antinodes, *this](const Pos antinode) {
        if (in_bounds(antinode)) {
            antinodes.insert(antinode);
        }
    };
    for (const auto &[_, positions] : antennas) {
        if (positions.size() < 2) {
            continue;
        }
        // loop over all combinations of pairs of antenna positions
        for (auto a = positions.begin(); a != positions.end(); ++a) {
            for (auto b = a + 1; b != positions.end(); ++b) {
                const Delta shift = *b - *a;
                add_antinode(*a - shift);
                add_antinode(*b + shift);
            }
        }
    }
    return antinodes;
}

} // namespace aoc::day08

#endif /* end of include guard: DAY08_HPP_0APLPOEJ */
