/******************************************************************************
 * File:        day12.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-12
 *****************************************************************************/

#include "lib.hpp"
#include <algorithm> // for copy, transform
#include <cassert>   // for assert
#include <cstddef>   // for size_t
#include <iomanip>   // for setw
#include <iostream>  // for cout, cerr, ostream, istream
#include <iterator>  // for back_inserter, distance
#include <sstream>   // for istringstream
#include <string>    // for string, getline
#include <utility>   // for move
#include <vector>    // for vector

#include "unit_test/pretty_print.hpp"

namespace aoc::day12 {

// Nonograms!

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

    bool
    are_positions_valid(const std::vector<std::size_t> &group_positions) const {
        assert(group_positions.size() > 0);
        assert(group_positions.size() <= groups.size());
        std::vector<Spring> candidate(springs.size(), Spring::good);
        auto it = candidate.begin();
        auto pos_it = group_positions.begin();
        auto len_it = groups.begin();
        for (; pos_it != group_positions.end() && len_it != groups.end();
             ++pos_it, ++len_it) {
            it = candidate.begin() + *pos_it;
            if (it != candidate.begin() && *(it - 1) != Spring::good) {
                if constexpr (aoc::DEBUG) {
                    std::cerr << "         ";
                    for (const Spring &s : candidate) {
                        std::cerr << static_cast<char>(s);
                    }
                    std::cerr << " (";
                    for (const auto &pos : group_positions) {
                        std::cerr << pos << " ";
                    }
                    std::cerr << "): missing gap before "
                              << std::distance(candidate.begin(), it) << "\n";
                }
                return false;
            }
            for (std::size_t i = 0; it != candidate.end() && i < *len_it;
                 ++it, ++i) {
                *it = Spring::bad;
            }
        }
        if (it != candidate.end()) {
            *it = Spring::good;
            ++it;
        }
        if (group_positions.size() < groups.size()) {
            for (; it != candidate.end(); ++it) {
                *it = Spring::unknown;
            }
        }
        bool valid = check_compatible(candidate, springs);
        if constexpr (aoc::DEBUG) {
            std::cerr << "checking ";
            for (const Spring &s : candidate) {
                std::cerr << static_cast<char>(s);
            }
            std::cerr << " (";
            for (const auto &pos : group_positions) {
                std::cerr << pos << " ";
            }
            std::cerr << "): " << (valid ? "valid" : "invalid") << "\n";
        }
        return valid;
    }
};

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

/// group_positions is a vector of length n <= rec.groups.size(), specifying
/// the starting positions of the first n groups.
int count_arrangements_rec(const ConditionRecord &rec,
                           std::vector<std::size_t> &group_positions) {
    const std::size_t group_idx = group_positions.size();
    if (group_idx == rec.groups.size()) {
        if constexpr (aoc::DEBUG) {
            std::cerr << std::setw(2) << "*"
                      << " ";
        }
        return rec.are_positions_valid(group_positions) ? 1 : 0;
    }
    int start_pos = 0;
    if (group_idx > 0) {
        start_pos = group_positions.back() + rec.groups.at(group_idx - 1) + 0;
    }
    int count = 0;
    const std::size_t length = rec.groups.at(group_idx);
    for (std::size_t pos = start_pos; pos <= rec.springs.size() - length;
         ++pos) {
        if (pos > 0 && rec.springs.at(pos - 1) == Spring::bad) {
            continue;
        }
        bool can_fit = true;
        for (std::size_t i = pos; i < pos + length; ++i) {
            if (rec.springs.at(i) == Spring::good) {
                can_fit = false;
                break;
            }
        }
        if (!can_fit) {
            continue;
        }
        group_positions.push_back(pos);
        if constexpr (aoc::DEBUG) {
            std::cerr << std::setw(2) << group_idx << " ";
        }
        if (rec.are_positions_valid(group_positions)) {
            count += count_arrangements_rec(rec, group_positions);
        }
        group_positions.pop_back();
    }

    return count;
}

int count_arrangements(const ConditionRecord &rec) {
    std::vector<std::size_t> positions;
    positions.reserve(rec.groups.size());
    return count_arrangements_rec(rec, positions);
}

} // namespace aoc::day12

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    auto records = aoc::day12::read_records(infile);

    int part_1 = 0;
    for (const auto &rec : records) {
        if constexpr (aoc::DEBUG) {
            std::cerr << "            " << rec << "\n";
        }
        part_1 += count_arrangements(rec);
        if constexpr (aoc::DEBUG) {
            std::cerr << "\n";
        }
    }

    std::cout << part_1 << "\n";

    return 0;
}
