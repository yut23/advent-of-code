/******************************************************************************
 * File:        day14.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-14
 *****************************************************************************/

#ifndef DAY14_HPP_A7H2MCKZ
#define DAY14_HPP_A7H2MCKZ

#include "ds/grid.hpp" // for Grid
#include "lib.hpp"     // for Pos
#include <cstddef>     // for size_t
#include <iostream>    // for istream
#include <string>      // for string, getline
#include <utility>     // for move
#include <vector>      // for vector

namespace aoc::day14 {

enum class Rock : char {
    empty = '.',
    round = 'O',
    cube = '#',
};

// N.B.: tilt direction is towards negative x (west in the original orientation)
struct Platform {
    aoc::ds::Grid<Rock> rocks;

  private:
    void transpose();
    void flip_rows();
    void flip_columns();

    void roll_row_down(int row);

  public:
    explicit Platform(std::vector<std::vector<Rock>> &&rocks_)
        : rocks(std::move(rocks_)) {}
    void rotate_cw();
    void rotate_ccw();
    void tilt();
    int calculate_load() const;
    std::vector<bool> round_rocks() const;
};

void Platform::transpose() {
    const int n = rocks.width;
    for (int y = 0; y < n; ++y) {
        for (int x = 0; x < y; ++x) {
            std::swap(rocks.at_unchecked(x, y), rocks.at_unchecked(y, x));
        }
    }
}

void Platform::flip_rows() {
    const int n = rocks.width;
    for (int y = 0; y < n; ++y) {
        for (int x = 0; x < n / 2; ++x) {
            const int z = n - x - 1;
            std::swap(rocks.at_unchecked(x, y), rocks.at_unchecked(z, y));
        }
    }
}

void Platform::flip_columns() {
    const int n = rocks.width;
    for (int x = 0; x < n; ++x) {
        for (int y = 0; y < n / 2; ++y) {
            const int z = n - y - 1;
            std::swap(rocks.at_unchecked(x, y), rocks.at_unchecked(x, z));
        }
    }
}

void Platform::rotate_cw() {
    transpose();
    flip_rows();
}

void Platform::rotate_ccw() {
    transpose();
    flip_columns();
}

void Platform::roll_row_down(int row) {
    std::size_t i = rocks.get_index(0, row);
    std::size_t end = i + rocks.width;
    std::size_t last_empty = end;
    for (; i != end; ++i) {
        switch (rocks[i]) {
        case Rock::round:
            if (last_empty != end) {
                std::swap(rocks[i], rocks[last_empty]);
                ++last_empty;
            }
            break;
        case Rock::cube:
            last_empty = end;
            break;
        case Rock::empty:
            if (last_empty == end) {
                last_empty = i;
            }
            break;
        }
    }
}

void Platform::tilt() {
    for (int row = 0; row < rocks.height; ++row) {
        roll_row_down(row);
    }
}

int Platform::calculate_load() const {
    int load = 0;
    rocks.for_each([&load, width = rocks.width](Rock rock, const Pos &pos) {
        if (rock == Rock::round) {
            load += width - pos.x;
        }
    });
    return load;
}

std::vector<bool> Platform::round_rocks() const {
    std::vector<bool> round(rocks.data().size(), false);
    for (std::size_t i = 0; i < rocks.data().size(); ++i) {
        if (rocks[i] == Rock::round) {
            round[i] = true;
        }
    }
    return round;
}

std::ostream &operator<<(std::ostream &os, const Platform &platform) {
    return platform.rocks.custom_print(
        os, [&os](Rock rock) { os << static_cast<char>(rock); });
}

Platform read_platform(std::istream &is) {
    std::vector<std::vector<Rock>> rocks;
    std::string line;
    while (std::getline(is, line)) {
        rocks.emplace_back();
        for (char ch : line) {
            rocks.back().push_back(Rock{ch});
        }
    }
    return Platform{std::move(rocks)};
}

} // namespace aoc::day14

#endif /* end of include guard: DAY14_HPP_A7H2MCKZ */
