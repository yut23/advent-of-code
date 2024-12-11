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
#include <array>    // for array
#include <cassert>  // for assert
#include <compare>  // for operator>= (iterator)
#include <iostream> // for istream, ostream, cerr
#include <iterator> // for next, prev, distance
#include <list>     // for list
#include <map>      // for map
#include <optional> // for optional
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

constexpr int MAX_FILE_SIZE = 9;

class FreeSpaceHelper {
    // store iterators to
    std::array<std::map<int, std::list<FileSpan>::iterator>, MAX_FILE_SIZE + 1>
        free_space;

  public:
    void mark_empty(std::list<FileSpan>::iterator it);
    void unmark_empty(std::list<FileSpan>::iterator it);
    std::optional<std::list<FileSpan>::iterator>
    find_free_space(std::list<FileSpan>::iterator from_it) const;

    friend struct DiskLayout;
};

struct DiskLayout {
  private:
    std::list<FileSpan> spans;

    void move_file(std::list<FileSpan>::iterator from,
                   std::list<FileSpan>::iterator to, FreeSpaceHelper &helper);

    void
    check_invariants(const std::list<FileSpan>::const_iterator &max_adjacent,
                     const FreeSpaceHelper &helper) const;

  public:
    static DiskLayout read(std::istream &is);
    DiskLayout compact_fragmented() const;
    DiskLayout compact_smart() const;
    long calculate_checksum() const;

    friend std::ostream &operator<<(std::ostream &, const DiskLayout &);
};

DiskLayout DiskLayout::read(std::istream &is) {
    DiskLayout l;
    char ch;
    int file_id = 0;
    bool is_empty = false;
    int pos = 0;
    while (is >> ch) {
        int count = ch - '0';
        int value = is_empty ? EMPTY : file_id++;
        if (count > 0) {
            l.spans.push_back({value, pos, count});
        }
        pos += count;
        is_empty = !is_empty;
    }
    while (l.spans.back().empty()) {
        l.spans.pop_back();
    }
    return l;
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
void DiskLayout::check_invariants(
    const std::list<FileSpan>::const_iterator &max_adjacent,
    const FreeSpaceHelper &helper) const {
    if constexpr (aoc::DEBUG) {
        int curr_pos = 0;
        bool check_adjacent = max_adjacent != spans.begin();
        for (auto it = spans.begin(); it != spans.end();
             curr_pos += it->size, ++it) {
            // no zero-length spans allowed
            assert(it->size > 0);
            // positions should match the sum of the previous sizes
            assert(it->pos == curr_pos);
            if (std::next(it) == max_adjacent) {
                check_adjacent = false;
            }
            if (check_adjacent) {
                // two adjacent spans should have different file IDs
                assert(it->file_id != std::next(it)->file_id);

                for (int size = 1; size <= MAX_FILE_SIZE; ++size) {
                    if (size > it->size || !it->empty()) {
                        // entry size should be <= size for map, and files
                        // should not appear in helper.free_space
                        assert(!helper.free_space[size].contains(it->pos));
                    } else {
                        // an empty span should have corresponding entries in
                        // helper.free_space
                        assert(helper.free_space[size].at(it->pos) == it);
                    }
                }
            }
        }

        // check helper invariants
        for (int size = 1; size <= MAX_FILE_SIZE; ++size) {
            for (const auto &[pos, it] : helper.free_space[size]) {
                // entry should be empty
                assert(it->empty());
                // position should match
                assert(it->pos == pos);
                // entry size should be at least as large as this map's size
                assert(it->size >= size);
            }
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
    return l;
}

void FreeSpaceHelper::mark_empty(std::list<FileSpan>::iterator it) {
    if constexpr (!aoc::FAST) {
        assert(it->empty());
    }
    for (int i = 1; i <= it->size; ++i) {
        free_space[i][it->pos] = it;
    }
}

void FreeSpaceHelper::unmark_empty(std::list<FileSpan>::iterator it) {
    for (int i = 1; i <= it->size; ++i) {
        free_space[i].erase(it->pos);
    }
}

std::optional<std::list<FileSpan>::iterator>
FreeSpaceHelper::find_free_space(std::list<FileSpan>::iterator from_it) const {
    if (free_space[from_it->size].empty()) {
        return {};
    }
    auto to_it = free_space[from_it->size].begin()->second;
    if (to_it->pos >= from_it->pos) {
        // only move files towards the beginning of the disk
        return {};
    }
    return to_it;
}

void DiskLayout::move_file(std::list<FileSpan>::iterator from,
                           std::list<FileSpan>::iterator to,
                           FreeSpaceHelper &helper) {
    check_invariants(from, helper);
    if constexpr (!aoc::FAST) {
        assert(to->empty());
        assert(!from->empty());
        assert(to->size >= from->size);
    }
    if constexpr (aoc::DEBUG) {
        std::cerr << "moving file " << from->file_id << " from " << from->pos
                  << " to empty space at " << to->pos << "\n";
    }

    helper.unmark_empty(to);
    if (to->size == from->size) {
        std::swap(to->file_id, from->file_id);
    } else {
        auto size = from->size;
        spans.insert(to, {from->file_id, to->pos, size});
        from->file_id = EMPTY;
        to->pos += size;
        to->size -= size;
        helper.mark_empty(to);
    }
    check_invariants(from, helper);
}

DiskLayout DiskLayout::compact_smart() const {
    // O(n log n) implementation: empty spans are stored in free_space, grouped
    // by the file sizes they can hold and ordered by position
    DiskLayout l{*this};
    FreeSpaceHelper helper;
    for (auto it = l.spans.begin(); it != l.spans.end(); ++it) {
        if (it->empty()) {
            helper.mark_empty(it);
        }
    }
    l.check_invariants(l.spans.end(), helper);
    // keep track of which file we're looking for, so we move files in order of
    // decreasing file ID
    int curr_file_id;
    {
        auto it = l.spans.end();
        do {
            --it;
        } while (it->empty());
        curr_file_id = it->file_id;
    }
    for (auto from_it = std::prev(l.spans.end()); from_it != l.spans.begin();
         --from_it) {
        if (from_it->file_id != curr_file_id) {
            continue;
        }
        // look up the first empty space where *from_it will fit
        auto maybe_to = helper.find_free_space(from_it);
        if (maybe_to.has_value()) {
            l.move_file(from_it, *maybe_to, helper);
        }
        // decrement the current file ID
        --curr_file_id;
    }

    l.check_invariants(l.spans.begin(), helper);
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
