/******************************************************************************
 * File:        day07.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-12-07
 *****************************************************************************/

#ifndef DAY07_HPP_XQTFHGV1
#define DAY07_HPP_XQTFHGV1

#include "ds/grid.hpp" // for Grid
#include "lib.hpp"     // for read_whole_stream
#include <cassert>     // for assert
#include <iomanip>     // for setw
#include <iostream>    // for ostream, istream
#include <iterator>    // for next
#include <string>      // for string
#include <utility>     // for swap, move
#include <vector>      // for vector

namespace aoc::day07 {

enum class TileType : char {
    EMPTY = '.',
    SPLITTER = '^',
    BEAM = '|',
};
struct Tile {
    TileType type;
    long visit_count = 0;

    Tile &operator+=(const Tile &other);
};

Tile &Tile::operator+=(const Tile &other) {
    if (type == TileType::EMPTY && other.type == TileType::BEAM) {
        type = TileType::BEAM;
    }
    visit_count += other.visit_count;
    return *this;
}

std::ostream &operator<<(std::ostream &os, const Tile &tile) {
    os << static_cast<char>(tile.type);
    return os;
}

class ManifoldGrid : public aoc::ds::Grid<Tile> {
    explicit ManifoldGrid(int width, int height,
                          aoc::ds::Grid<Tile>::container_type &&data)
        : aoc::ds::Grid<Tile>(width, height, std::move(data)) {}

  public:
    static ManifoldGrid read(std::istream &is);
    int propagate_tachyon_beams();
    long count_timelines() const;
    void print_visits(std::ostream &os) const;
};

ManifoldGrid ManifoldGrid::read(std::istream &is) {
    std::string data = aoc::read_whole_stream(is);
    int width = data.find_first_of('\n');
    assert(data.length() % (width + 1) == 0);
    int height = data.length() / (width + 1);
    std::vector<Tile> transformed{};
    transformed.reserve(height * width);
    for (char c : data) {
        if (c == 'S') {
            transformed.push_back({TileType::BEAM, 1});
        } else if (c != '\n') {
            transformed.push_back({TileType(c)});
        }
    }
    return ManifoldGrid(width, height, std::move(transformed));
}

int ManifoldGrid::propagate_tachyon_beams() {
    ManifoldGrid::iterator prev_row_it = begin();
    ManifoldGrid::iterator curr_row_it = prev_row_it + 1;
    const ManifoldGrid::iterator row_end = end();
    int split_count = 0;
    while (curr_row_it != row_end) {
        auto prev_row = *prev_row_it, curr_row = *curr_row_it;
        for (int i = 0; i < width; ++i) {
            using enum TileType;
            Tile &curr = curr_row[i];
            Tile &prev = prev_row[i];
            if (prev.type != BEAM) {
                continue;
            }
            curr += prev;
            if (curr.type == SPLITTER) {
                assert(i > 0);
                assert(i < width - 1);
                curr_row[i - 1] += prev;
                curr_row[i + 1] += prev;
                ++split_count;
            }
        }

        // leapfrog the two iterators
        prev_row_it += 2;
        std::swap(curr_row_it, prev_row_it);
    }
    return split_count;
}

long ManifoldGrid::count_timelines() const {
    const auto last_row = *(std::next(this->end(), -1));
    long count = 0;
    for (int i = 0; i < width; ++i) {
        count += last_row[i].visit_count;
    }
    return count;
}

void ManifoldGrid::print_visits(std::ostream &os) const {
    custom_print(os, [&os](const auto tile) {
        os << std::setw(2);
        using enum TileType;
        switch (tile.type) {
        case EMPTY:
            os << ' ';
            break;
        case SPLITTER:
            os << '^';
            break;
        case BEAM:
            os << tile.visit_count;
            break;
        }
        os << ' ';
    });
}

std::ostream &operator<<(std::ostream &os, const ManifoldGrid &grid) {
    return grid.custom_print(
        os, [&os](const auto tile) { os << static_cast<char>(tile.type); });
}

} // namespace aoc::day07

#endif /* end of include guard: DAY07_HPP_XQTFHGV1 */
