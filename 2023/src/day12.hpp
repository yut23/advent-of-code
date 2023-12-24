/******************************************************************************
 * File:        day12.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-12
 *****************************************************************************/

#ifndef DAY12_HPP_5NZ0FAOX
#define DAY12_HPP_5NZ0FAOX

#include "lib.hpp"   // for skip, DEBUG
#include <algorithm> // for copy, find, transform
#include <cstddef>   // for size_t
#include <iostream>  // for cerr, ostream, istream
#include <iterator>  // for back_inserter, distance
#include <map>       // for map
#include <sstream>   // for istringstream
#include <string>    // for string, getline
#include <utility>   // for move, pair, make_pair
#include <vector>    // for vector

// Nonograms!

namespace aoc::day12 {

enum class Spring : char {
    good = '.',
    bad = '#',
    unknown = '?',
};

bool check_compatible(const std::vector<Spring> &s1,
                      const std::vector<Spring> &s2) {
    if (s1.size() != s2.size()) {
        return false;
    }
    for (std::size_t i = 0; i < s1.size(); ++i) {
        if (s1[i] == Spring::unknown || s2[i] == Spring::unknown) {
            continue;
        }
        if (s1[i] != s2[i]) {
            return false;
        }
    }
    return true;
}

struct ConditionRecord {
    std::vector<Spring> springs;
    std::vector<std::size_t> groups;

  public:
    long count_arrangements(std::size_t spring_idx = 0,
                            std::size_t group_idx = 0, int depth = 0) const;

    ConditionRecord repeat(int count) const;

  private:
    mutable std::map<std::pair<std::size_t, std::size_t>, long> memo{};
};

long ConditionRecord::count_arrangements(std::size_t spring_idx,
                                         std::size_t group_idx,
                                         int depth) const {
    if constexpr (aoc::DEBUG) {
        std::cerr << std::string(depth * 2, ' ')
                  << "entering count_arrangements(" << spring_idx << ", "
                  << group_idx << ")\n";
    }
    std::string pad((depth + 1) * 2, ' ');
    if (group_idx == groups.size()) {
        auto it = springs.end();
        if (spring_idx < springs.size()) {
            it = std::find(springs.begin() + spring_idx, springs.end(),
                           Spring::bad);
        }
        long count = it == springs.end() ? 1 : 0;
        if constexpr (aoc::DEBUG) {
            if (count == 0) {
                std::cerr << pad << "found unmatched bad spring at index "
                          << std::distance(springs.begin(), it) << "\n";
            } else {
                std::cerr << pad << "got final count = 1\n";
            }
        }
        return count;
    }

    while (spring_idx < springs.size() && springs[spring_idx] == Spring::good) {
        ++spring_idx;
    }

    if (spring_idx >= springs.size()) {
        if constexpr (aoc::DEBUG) {
            std::cerr << pad << "reached end of springs with "
                      << (groups.size() - group_idx) << " groups remaining\n";
        }
        return 0;
    }

    auto key = std::make_pair(spring_idx, group_idx);
    auto it = memo.find(key);
    if (it != memo.end()) {
        if constexpr (aoc::DEBUG) {
            std::cerr << pad << "looked up count = " << it->second << "\n";
        }
        return it->second;
    }

    long count = 0;

    // try placing the next group at spring_idx
    if (spring_idx + groups[group_idx] <= springs.size()) {
        bool valid = true;
        std::size_t i = spring_idx;
        if constexpr (aoc::DEBUG) {
            std::cerr << pad << "trying group {" << groups[group_idx]
                      << "} at index " << i << "...\n";
        }
        for (; i < spring_idx + groups[group_idx]; ++i) {
            // check that the entire group can be bad springs
            if (springs[i] == Spring::good) {
                valid = false;
                if constexpr (aoc::DEBUG) {
                    std::cerr << pad << "  failed: found good spring at index "
                              << i << "\n";
                }
                break;
            }
        }
        i = spring_idx + groups[group_idx];
        // check that the end of the group can be a good spring
        // cppcheck-suppress knownConditionTrueFalse; false positive
        if (valid && i < springs.size() && springs[i] == Spring::bad) {
            if constexpr (aoc::DEBUG) {
                std::cerr << pad
                          << "  failed: found bad spring after group at index "
                          << i << "\n";
            }
            valid = false;
        }
        if (valid) {
            // recurse at the new spring index and the next group
            count = count_arrangements(i + 1, group_idx + 1, depth + 1);
        }
    }

    if (springs[spring_idx] == Spring::unknown) {
        // try with spring_idx as a good spring
        if constexpr (aoc::DEBUG) {
            std::cerr << pad << "trying at next index...\n";
        }
        count += count_arrangements(spring_idx + 1, group_idx, depth + 1);
    }

    if constexpr (aoc::DEBUG) {
        std::cerr << pad << "got count = " << count << "\n";
    }
    memo.try_emplace(std::move(key), count);

    return count;
}

ConditionRecord ConditionRecord::repeat(int count) const {
    ConditionRecord new_rec;
    new_rec.springs = springs;
    new_rec.groups = groups;
    for (int i = 1; i < count; ++i) {
        new_rec.springs.push_back(Spring::unknown);
        std::copy(springs.begin(), springs.end(),
                  std::back_inserter(new_rec.springs));
        std::copy(groups.begin(), groups.end(),
                  std::back_inserter(new_rec.groups));
    }
    return new_rec;
}

std::vector<ConditionRecord> read_records(std::istream &is) {
    std::vector<ConditionRecord> records;

    std::string tmp;
    while (std::getline(is, tmp)) {
        ConditionRecord record;
        std::istringstream ss{tmp};
        ss >> tmp;
        std::transform(tmp.begin(), tmp.end(),
                       std::back_inserter(record.springs),
                       [](char ch) { return Spring(ch); });
        std::size_t x;
        ss >> x;
        do {
            record.groups.push_back(x);
        } while (ss >> aoc::skip<char>(1) >> x);
        records.push_back(std::move(record));
    }

    return records;
}

std::ostream &operator<<(std::ostream &os, const ConditionRecord &rec) {
    for (const Spring &s : rec.springs) {
        os << static_cast<char>(s);
    }
    os << " ";
    for (auto it = rec.groups.begin(); it != rec.groups.end(); ++it) {
        if (it != rec.groups.begin()) {
            os << ",";
        }
        os << *it;
    }
    return os;
}

} // namespace aoc::day12

#endif /* end of include guard: DAY12_HPP_5NZ0FAOX */
