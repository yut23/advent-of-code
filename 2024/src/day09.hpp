/******************************************************************************
 * File:        day09.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-09
 *****************************************************************************/

#ifndef DAY09_HPP_NKM6TGY7
#define DAY09_HPP_NKM6TGY7

#include "lib.hpp"  // for FAST
#include <cassert>  // for assert
#include <compare>  // for operator>= (iterator)
#include <cstddef>  // for size_t
#include <iostream> // for istream, ostream
#include <utility>  // for swap
#include <vector>   // for vector

namespace aoc::day09 {

constexpr int EMPTY = -1;

class DiskLayout {
    std::vector<int> blocks;

  public:
    DiskLayout() = default;

    void compact();
    long calculate_checksum() const;

    friend std::istream &operator>>(std::istream &, DiskLayout &);
    friend std::ostream &operator<<(std::ostream &, const DiskLayout &);
};

std::istream &operator>>(std::istream &is, DiskLayout &disk_layout) {
    char ch;
    int file_id = 0;
    bool is_empty = false;
    while (is >> ch) {
        int count = ch - '0';
        int value = is_empty ? EMPTY : file_id++;
        disk_layout.blocks.resize(disk_layout.blocks.size() + count, value);
        is_empty = !is_empty;
    }
    return is;
}

std::ostream &operator<<(std::ostream &os, const DiskLayout &disk_layout) {
    os << '{';
    for (int block : disk_layout.blocks) {
        if (block == EMPTY) {
            os << '.';
        } else {
            os << '[' << block << ']';
        }
    }
    os << '}';
    return os;
}

// disk_layout is passed by value to make a copy
void DiskLayout::compact() {
    auto first_empty = blocks.begin();
    auto last_filled = blocks.end() - 1;

    while (true) {
        while (first_empty != blocks.end() && *first_empty != EMPTY) {
            ++first_empty;
        }
        while (last_filled >= blocks.begin() && *last_filled == EMPTY) {
            --last_filled;
        }
        if (first_empty >= last_filled) {
            // all empty spaces have been filled
            break;
        }
        // move last filled block to first empty space
        std::swap(*first_empty, *last_filled);
        ++first_empty;
        --last_filled;
    }

    // remove trailing empty blocks
    blocks.erase(last_filled + 1, blocks.end());
}

long DiskLayout::calculate_checksum() const {
    long checksum = 0;
    for (std::size_t i = 0; i < blocks.size(); ++i) {
        if constexpr (!aoc::FAST) {
            assert(blocks[i] != EMPTY);
        }
        checksum += i * blocks[i];
    }
    return checksum;
}

} // namespace aoc::day09

#endif /* end of include guard: DAY09_HPP_NKM6TGY7 */
