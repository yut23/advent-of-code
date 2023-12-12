/******************************************************************************
 * File:        day11.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-11
 *****************************************************************************/

#include "lib.hpp"  // for Pos
#include <iostream> // for istream
#include <set>      // for set
#include <string>   // for string, getline
#include <vector>   // for vector

namespace aoc::day11 {

void expand(std::vector<Pos> &galaxies) {
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
        int dx = nonempty_columns.contains(x) ? 1 : 2;
        expanded_x.push_back(expanded_x.back() + dx);
    }

    std::vector<int> expanded_y{0};
    for (int y = 1; y <= max_y; ++y) {
        int dy = nonempty_rows.contains(y) ? 1 : 2;
        expanded_y.push_back(expanded_y.back() + dy);
    }

    // update the galaxy positions
    for (Pos &pos : galaxies) {
        pos.x = expanded_x[pos.x];
        pos.y = expanded_y[pos.y];
    }
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
