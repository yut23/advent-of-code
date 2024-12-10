/******************************************************************************
 * File:        day09.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-09
 *****************************************************************************/

#ifndef DAY09_HPP_NKM6TGY7
#define DAY09_HPP_NKM6TGY7

#include "lib.hpp" // for FAST, DEBUG
#include "unit_test/pretty_print.hpp"
#include <cassert>  // for assert
#include <compare>  // for operator>= (iterator)
#include <iostream> // for istream, ostream, cerr
#include <iterator> // for next, prev
#include <list>     // for list
#include <utility>  // for swap, move
#include <vector>   // for vector

namespace aoc::day09 {

constexpr int EMPTY = -1;

struct FileSpan {
    int file_id;
    int pos;
    int size;

    bool empty() const { return file_id == EMPTY; }
};

std::ostream &operator<<(std::ostream &os, const FileSpan &fs) {
    os << "FileSpan(id=";
    if (fs.empty()) {
        os << "EMPTY";
    } else {
        os << fs.file_id;
    }
    os << ", pos=" << fs.pos << ", size=" << fs.size << ")";
    return os;
}

struct DiskLayout {
  private:
    std::list<FileSpan> spans;

    /// returns an iterator to the span before `from`
    std::list<FileSpan>::iterator move_file(std::list<FileSpan>::iterator from,
                                            std::list<FileSpan>::iterator to);

    void check_invariants() const;

  public:
    DiskLayout compact_fragmented() const;
    DiskLayout compact_smart() const;
    long calculate_checksum() const;

    friend std::istream &operator>>(std::istream &, DiskLayout &);
    friend std::ostream &operator<<(std::ostream &, const DiskLayout &);
};

std::istream &operator>>(std::istream &is, DiskLayout &disk_layout) {
    char ch;
    int file_id = 0;
    bool is_empty = false;
    int pos = 0;
    while (is >> ch) {
        int count = ch - '0';
        int value = is_empty ? EMPTY : file_id++;
        if (count > 0) {
            disk_layout.spans.push_back({value, pos, count});
        }
        pos += count;
        is_empty = !is_empty;
    }
    while (disk_layout.spans.back().empty()) {
        disk_layout.spans.pop_back();
    }
    return is;
}

std::ostream &operator<<(std::ostream &os, const DiskLayout &disk_layout) {
    os << '[';
    for (const auto &span : disk_layout.spans) {
        assert(span.size > 0);
        for (int i = 0; i < span.size; ++i) {
            if (span.file_id == EMPTY) {
                os << '.';
            } else {
                os << span.file_id;
            }
        }
    }
    os << ']';
    os << " " << pretty_print::repr(disk_layout.spans);
    return os;
}

// check some invariants that should hold at all times
void DiskLayout::check_invariants() const {
    if constexpr (!aoc::DEBUG) {
        return;
    }
    int curr_pos = 0;
    for (auto it = spans.begin(); it != spans.end();
         curr_pos += it->size, ++it) {
        // no zero-length spans allowed
        assert(it->size > 0);
        // positions should match the sum of the previous sizes
        assert(it->pos == curr_pos);
        if (std::next(it) != spans.end()) {
            // two adjacent spans should have different file IDs
            assert(it->file_id != std::next(it)->file_id);
        }
    }
}

DiskLayout DiskLayout::compact_fragmented() const {
    if (spans.empty()) {
        return {};
    }
    // convert to raw blocks
    std::vector<int> blocks{};
    for (const auto &span : spans) {
        blocks.resize(blocks.size() + span.size, span.file_id);
    }
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

    // convert back to spans
    DiskLayout l;
    if (blocks.empty()) {
        return l;
    }
    auto curr_span = l.spans.insert(l.spans.end(), {blocks[0], 0, 1});
    for (int i = 1; i < static_cast<int>(blocks.size()); ++i) {
        int file_id = blocks[i];
        if (file_id == curr_span->file_id) {
            ++curr_span->size;
        } else {
            curr_span = l.spans.insert(l.spans.end(), {file_id, i, 1});
        }
    }
    l.check_invariants();
    return l;
}

std::list<FileSpan>::iterator
DiskLayout::move_file(std::list<FileSpan>::iterator from,
                      std::list<FileSpan>::iterator to) {
    check_invariants();
    if constexpr (!aoc::FAST) {
        assert(to->empty());
        assert(!from->empty());
        assert(to->size >= from->size);
    }
    if constexpr (aoc::DEBUG) {
        std::cerr << "moving file " << from->file_id << " from " << from->pos
                  << " to empty space at " << to->pos << "\n";
    }

    std::list<FileSpan>::iterator new_from;
    if (std::prev(from)->empty()) {
        // `from` is preceded by an empty span, so expand it to fill the space
        new_from = std::prev(from);
        new_from->size += from->size;
    } else {
        // insert an empty span to take the place of *from
        new_from = spans.insert(from, *from);
        new_from->file_id = EMPTY;
    }
    // move *from to right before `to`
    spans.splice(to, spans, from);
    from->pos = to->pos;
    // adjust *to
    to->pos += from->size;
    to->size -= from->size;
    if (to->size == 0) {
        // remove *to if necessary
        spans.erase(to);
    }
    // `to` should now be treated as invalid

    if (std::next(new_from)->empty()) {
        // `new_from` is followed by an empty span, so combine them
        new_from->size += std::next(new_from)->size;
        spans.erase(std::next(new_from));
    }

    check_invariants();
    return new_from;
}

DiskLayout DiskLayout::compact_smart() const {
    check_invariants();
    // simple O(n^2) algorithm: linear search for the first empty block where
    // the current file will fit
    DiskLayout l{*this};
    // keep track of which file we're looking for, so we move files in order of
    // decreasing file ID
    int curr_file_id = EMPTY;
    for (auto from_it = std::prev(l.spans.end()); from_it != l.spans.begin();
         --from_it) {
        if (from_it->empty()) {
            continue;
        }
        if (curr_file_id == EMPTY) {
            curr_file_id = from_it->file_id;
        }
        if (from_it->file_id != curr_file_id) {
            continue;
        }
        // search for an empty space where *from_it will fit, starting from the
        // beginning of the filesystem
        for (auto to_it = l.spans.begin(); to_it != from_it; ++to_it) {
            if (!to_it->empty() || to_it->size < from_it->size) {
                continue;
            }
            from_it = l.move_file(from_it, to_it);
            break;
        }
        // decrement the current file ID
        --curr_file_id;
    }

    l.check_invariants();
    return l;
}

long DiskLayout::calculate_checksum() const {
    long checksum = 0;
    long n = 0;
    for (const auto &span : spans) {
        if (!span.empty()) {
            long m = n + span.size;
            // $\sum_{i=n}^m (i-1) = \sum_{i=1}^m (i-1) - \sum_{i=1}^n (i-1)$
            checksum += span.file_id * (m * (m - 1) - n * (n - 1)) / 2;
        }
        n += span.size;
    }
    return checksum;
}

} // namespace aoc::day09

#endif /* end of include guard: DAY09_HPP_NKM6TGY7 */
