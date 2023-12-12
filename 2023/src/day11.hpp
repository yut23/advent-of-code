/******************************************************************************
 * File:        day11.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-11
 *****************************************************************************/

#include "lib.hpp"   // for Pos
#include <algorithm> // for transform
#include <iostream>  // for istream
#include <set>       // for set
#include <string>    // for string, getline
#include <vector>    // for vector

namespace aoc::day11 {

std::vector<Pos> expand(const std::vector<Pos> &galaxies, int amount) {
    // enumerate all rows and columns containing galaxies
    std::set<int> nonempty_columns;
    std::set<int> nonempty_rows;
    int max_x = 0;
    int max_y = 0;
    for (const Pos &pos : galaxies) {
        nonempty_columns.insert(pos.x);
        nonempty_rows.insert(pos.y);
        if (pos.x > max_x) {
            max_x = pos.x;
        }
        if (pos.y > max_y) {
            max_y = pos.y;
        }
    }

    // calculate the new x and y positions after expanding
    std::vector<int> expanded_x{0};
    for (int x = 1; x <= max_x; ++x) {
        int dx = nonempty_columns.contains(x) ? 1 : amount;
        expanded_x.push_back(expanded_x.back() + dx);
    }

    std::vector<int> expanded_y{0};
    for (int y = 1; y <= max_y; ++y) {
        int dy = nonempty_rows.contains(y) ? 1 : amount;
        expanded_y.push_back(expanded_y.back() + dy);
    }

    // update the galaxy positions
    std::vector<Pos> new_galaxies(galaxies);
    std::ranges::transform(new_galaxies, new_galaxies.begin(),
                           [&expanded_x, &expanded_y](const Pos &pos) {
                               return Pos{expanded_x[pos.x], expanded_y[pos.y]};
                           });
    return new_galaxies;
}

std::vector<Pos> read_image(std::istream &is) {
    std::vector<Pos> galaxies;
    Pos pos{0, 0};
    // find all galaxies
    std::string line;
    while (std::getline(is, line)) {
        for (char ch : line) {
            if (ch == '#') {
                galaxies.push_back(pos);
            }
            ++pos.x;
        }
        pos.x = 0;
        ++pos.y;
    }
    return galaxies;
}

} // namespace aoc::day11
