/******************************************************************************
 * File:        day05.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-05
 *****************************************************************************/

#ifndef DAY05_HPP_BJU0DY58
#define DAY05_HPP_BJU0DY58

#include "lib.hpp"   // for skip
#include <algorithm> // for find_if, transform, max, min
#include <iostream>  // for istream, ostream, ws
#include <sstream>   // for istringstream
#include <string>    // for string, getline
#include <utility>   // for move, forward
#include <vector>    // for vector

namespace aoc::day05 {

struct Range {
    long start{0};
    long length{0};

    Range() = default;
    Range(long start, long length) : start(start), length(length) {}

    bool contains(long value) const {
        return value >= start && value < start + length;
    }

    auto operator<=>(const Range &) const = default;
};

std::ostream &operator<<(std::ostream &os, const Range &r) {
    os << "[" << r.start << ", " << r.start + r.length << ")";
    return os;
}

struct MapEntry : public Range {
    long shift{0};

    MapEntry() = default;
    MapEntry(long dest_start, long src_start, long length)
        : Range(src_start, length), shift(dest_start - src_start) {}

    Range intersect(const Range &r) const {
        long max_start = std::max(start, r.start);
        long min_end = std::min(start + length, r.start + r.length);
        long new_length = min_end - max_start;
        if (new_length > 0) {
            return Range(max_start + shift, new_length);
        }
        return {};
    }

    auto operator<=>(const MapEntry &other) const = default;
};

std::istream &operator>>(std::istream &is, MapEntry &entry) {
    long dest_start, src_start, length;
    is >> dest_start >> src_start >> length;
    entry = MapEntry(dest_start, src_start, length);
    return is;
}
std::ostream &operator<<(std::ostream &os, const MapEntry &entry) {
    os << "MapEntry<[" << entry.start << ", " << entry.start + entry.length
       << "), shift=" << entry.shift << ">";
    return os;
}

struct ConversionMap {
    std::string label{};
    std::vector<MapEntry> entries;

    ConversionMap() = default;

    template <class T>
    ConversionMap(const std::string &label, T &&entries)
        : label(label), entries(std::forward<std::vector<MapEntry>>(entries)) {}

    std::string get_label() const { return label; }
    void add_entry(MapEntry &&entry) { entries.push_back(std::move(entry)); }

    static ConversionMap read(std::istream &is) {
        ConversionMap map;
        // read label line, then consume the newline
        is >> map.label >> aoc::skip(1) >> std::ws;
        // read entries
        std::string line;
        while (std::getline(is, line) && !line.empty()) {
            std::istringstream ss{line};
            MapEntry entry;
            if (ss >> entry) {
                map.entries.push_back(std::move(entry));
            }
        }
        std::ranges::sort(map.entries);
        return map;
    }

    long apply(long source) const {
        auto entry_it =
            std::ranges::find_if(entries, [source](const MapEntry &entry) {
                return entry.contains(source);
            });
        if (entry_it != entries.end()) {
            return source + entry_it->shift;
        }
        return source;
    }

    void apply_in_place(std::vector<long> &inputs) const {
        std::ranges::transform(inputs, inputs.begin(),
                               [this](long src) { return apply(src); });
    }

    bool operator==(const ConversionMap &) const = default;
    friend std::ostream &operator<<(std::ostream &, const ConversionMap &);
};

std::ostream &operator<<(std::ostream &os, const ConversionMap &map) {
    os << "ConversionMap(" << map.label << ",\n";
    for (const auto &entry : map.entries) {
        os << "  " << entry << ",\n";
    }
    os << ")";
    return os;
}

std::vector<long> read_seeds(std::istream &is) {
    std::string line;
    std::getline(is, line);
    std::istringstream ss{line};
    // skip "seeds:"
    ss >> aoc::skip(1);

    std::vector<long> seeds;
    long seed;
    while (ss >> seed) {
        seeds.emplace_back(seed);
    }
    return seeds;
}

} // namespace aoc::day05

#endif /* end of include guard: DAY05_HPP_BJU0DY58 */
