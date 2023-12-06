/******************************************************************************
 * File:        day05.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-05
 *****************************************************************************/

#include "lib.hpp"   // for skip
#include <algorithm> // for find_if, transform
#include <iomanip>   // for setw
#include <iostream>  // for istream, ostream
#include <sstream>   // for istringstream
#include <string>    // for string, getline
#include <utility>   // for move
#include <vector>    // for vector

namespace aoc::day05 {

using id_t = long;

struct MapEntry {
    id_t src_start;
    id_t dest_start;
    id_t length;

    auto operator<=>(const MapEntry &) const = default;
};

std::istream &operator>>(std::istream &is, MapEntry &entry) {
    is >> entry.dest_start >> entry.src_start >> entry.length;
    return is;
}
std::ostream &operator<<(std::ostream &os, const MapEntry &entry) {
    os << "MapEntry<[" << std::setw(10) << entry.src_start << ", "
       << std::setw(10) << entry.src_start + entry.length << ") → ["
       << std::setw(10) << entry.dest_start << ", " << std::setw(10)
       << entry.dest_start + entry.length << ")>";
    return os;
}

class ConversionMap {
    std::string label{};
    std::vector<MapEntry> entries;

  public:
    ConversionMap() = default;
    explicit ConversionMap(const std::string &label) : label(label) {}
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
        // std::ranges::sort(map.entries);
        return map;
    }

    id_t apply(id_t source) const {
        // auto entry_it =
        //     std::ranges::find_if(entries, [source](const MapEntry &entry) {
        //         return source >= entry.src_start &&
        //                source < entry.src_start + entry.length;
        //     });
        // if (entry_it != entries.end()) {
        //     return source - entry_it->src_start + entry_it->dest_start;
        // }
        for (const auto &entry : entries) {
            if (source >= entry.src_start &&
                source < entry.src_start + entry.length) {
                return source - entry.src_start + entry.dest_start;
            }
        }
        return source;
    }

    std::vector<id_t> apply(const std::vector<id_t> &inputs) const {
        std::vector<id_t> outputs;
        for (auto src : inputs) {
            outputs.push_back(apply(src));
        }
        return outputs;
    }

    void apply_in_place(std::vector<id_t> &inputs) const {
        // std::ranges::transform(inputs, inputs.begin(),
        //                        [=](id_t src) { return apply(src); });
        for (auto &src : inputs) {
            src = apply(src);
        }
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

std::vector<id_t> read_seeds(std::istream &is) {
    std::string line;
    std::getline(is, line);
    std::istringstream ss{line};
    // skip "seeds:"
    ss >> aoc::skip(1);

    std::vector<id_t> seeds;
    id_t seed;
    while (ss >> seed) {
        seeds.emplace_back(seed);
    }
    return seeds;
}

} // namespace aoc::day05
