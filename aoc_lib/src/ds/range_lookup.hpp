#ifndef RANGE_LOOKUP_HPP_IXYKFQRO
#define RANGE_LOOKUP_HPP_IXYKFQRO

#include "lib.hpp"   // for DEBUG
#include <cassert>   // for assert
#include <iostream>  // for ostream, cerr
#include <iterator>  // for next
#include <map>       // for map
#include <memory>    // for shared_ptr, make_shared
#include <stdexcept> // for domain_error
#include <utility>   // for pair (from map)

namespace aoc::ds {

// represents the range from start to end-1
struct RangeElement {
    const long start;
    long end;

    RangeElement(long start, long end) : start(start), end(end) {
        if (end <= start) {
            throw std::domain_error("end must be strictly greater than start");
        }
    }

    long length() const { return end - start; }

    bool is_start(long index) const { return index == start; }
    bool is_end(long index) const { return index == end; }
};

std::ostream &operator<<(std::ostream &os, const RangeElement &elt) {
    os << '[' << elt.start << ", " << elt.end << ')';
    return os;
}

// stores a set of covered ranges; supports adding new ranges and checking point
// membership
class RangeLookup {
    using pointer = std::shared_ptr<RangeElement>;
    using iterator = std::map<long, pointer>::iterator;
    using const_iterator = std::map<long, pointer>::const_iterator;

    // ordered mapping of the start and end indices of a range to the
    // corresponding RangeElement object
    std::map<long, pointer> table{};

    // moves old_end to new_end, removes any subsumed ranges in between, and
    // merges with the range starting at new_end if present
    void expand_range(const_iterator start, long new_end);

    // constructs a new RangeElement and inserts it into the table
    iterator create_new_range(long start);

    // internal validity check
    void check_invariants() const;

  public:
    void add_range(long start, long end);
    long count_covered() const;
    bool is_covered(long index) const;

    friend std::ostream &operator<<(std::ostream &, const RangeLookup &);
};

std::ostream &operator<<(std::ostream &os, const RangeLookup &lookup) {
    bool first = true;
    for (const auto &[index, elt] : lookup.table) {
        if (elt->is_start(index)) {
            if (!first) {
                os << "\n";
            }
            os << "[" << index << ", ";
        } else {
            assert(elt->is_end(index));
            os << index - 1 << ")";
        }
        first = false;
    }
    return os;
}

void RangeLookup::add_range(long start, long end) {
    if constexpr (aoc::DEBUG) {
        std::cerr << "adding range [" << start << ", " << end << ")\n";
    }
    check_invariants();
    const_iterator lower_bound = table.lower_bound(start);
    if (lower_bound == table.end()) {
        if constexpr (aoc::DEBUG) {
            std::cerr << "at the end of the table, creating a new range\n";
        }
        expand_range(create_new_range(start), end);
    } else {
        long lower_pos = lower_bound->first;
        pointer lower_range = lower_bound->second;
        if (lower_range->is_start(lower_pos) && lower_pos != start) {
            if constexpr (aoc::DEBUG) {
                std::cerr << "outside existing ranges, creating a new range\n";
            }
            expand_range(create_new_range(start), end);
        } else {
            // update the end point of the existing range
            if constexpr (aoc::DEBUG) {
                std::cerr << "inside existing range, merging\n";
            }
            const_iterator start_it = table.find(lower_range->start);
            expand_range(start_it, end);
        }
    }
    check_invariants();
}

RangeLookup::iterator RangeLookup::create_new_range(long start) {
    pointer elt = std::make_shared<RangeElement>(start, start + 1);
    return table.emplace(start, elt).first;
}

void RangeLookup::expand_range(const_iterator start_it, long new_end) {
    const pointer elt = start_it->second;
    if (new_end < elt->end) {
        if constexpr (aoc::DEBUG) {
            std::cerr
                << "new range is entirely within existing one, returning\n";
        }
        return;
    }
    const_iterator end_it = table.find(elt->end);
    if (end_it != table.end() && end_it->second == elt) {
        // remove old end pointer for this range
        if constexpr (aoc::DEBUG) {
            std::cerr << "removing old end pointer at " << elt->end << "\n";
        }
        table.erase(end_it);
    }
    elt->end = new_end;
    // remove any ranges that the new range will overlap with
    while (true) {
        const_iterator it = std::next(start_it);
        if (it == table.end() || it->first > new_end) {
            break;
        }
        assert(it->second->is_start(it->first));
        // range to subsume or merge
        if (it->second->end > new_end) {
            // update new_end in order to merge with the overlapping range
            new_end = it->second->end;
            if constexpr (aoc::DEBUG) {
                std::cerr << "merging range " << *(it->second) << " into "
                          << *elt << " (new_end=" << new_end << ")\n";
            }
        } else if constexpr (aoc::DEBUG) {
            std::cerr << "subsuming range " << *(it->second) << "\n";
        }
        // remove subsumed/merged range
        table.erase(it, std::next(it, 2));
    }
    // update end value
    elt->end = new_end;
    // insert end pointer at new_end, and make sure the insert was successful
    assert(table.emplace(new_end, elt).second);
}

void RangeLookup::check_invariants() const {
    assert(table.size() % 2 == 0);
    if constexpr (aoc::DEBUG) {
        // std::cerr << "checking invariants...\n";
        for (const_iterator it = table.cbegin(); it != table.cend(); ++it) {
            // std::cerr << "  checking " << *(it->second) << " at " <<
            // it->first << "\n";
            const auto &[index, elt] = *it;
            long other_index;
            const_iterator other_it = it;
            if (index == elt->start) {
                ++other_it;
                other_index = elt->end;
            } else {
                --other_it;
                other_index = elt->start;
                // make sure different ranges aren't adjacent
                const_iterator next_it = std::next(it);
                if (next_it != table.end()) {
                    assert(next_it->first > index);
                }
            }
            assert(other_it != table.end());
            // std::cerr << "  other_index: " << other_index << "\n";
            // std::cerr << "  other_it: " << other_it->first << " -> "
            //           << *(other_it->second) << "\n";
            assert(other_it->first == other_index);
            assert(other_it->second == elt);
            assert(table.find(other_index) == other_it);
        }
        // std::cerr << "\n";
    }
}

long RangeLookup::count_covered() const {
    long count = 0;
    for (const auto &[index, elt] : table) {
        if (elt->is_start(index)) {
            count += elt->length();
        }
    }
    return count;
}

bool RangeLookup::is_covered(long index) const {
    // get the next entry after index
    auto it = table.upper_bound(index);
    if (it == table.end()) {
        // outside the right-most range
        return false;
    }
    // if the next entry is the end of a range, then index is inside that range
    return it->second->is_end(it->first);
}

} // namespace aoc::ds

#endif /* end of include guard: RANGE_LOOKUP_HPP_IXYKFQRO */
