/******************************************************************************
 * File:        day11.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-12-11
 *****************************************************************************/

#ifndef DAY11_HPP_0DCFGKPU
#define DAY11_HPP_0DCFGKPU

#include "graph_traversal.hpp" // for topo_sort
#include "lib.hpp"             // for read_vector, DEBUG

#include <algorithm>   // for ranges::for_each
#include <cassert>     // for assert
#include <cstddef>     // for size_t
#include <cstdint>     // for uint16_t
#include <functional>  // for less
#include <iomanip>     // for setw
#include <iostream>    // for istream, cerr
#include <map>         // for map
#include <set>         // for set
#include <sstream>     // for istringstream
#include <stdexcept>   // for out_of_range
#include <string>      // for string, getline
#include <string_view> // for string_view
#include <utility>     // for pair (map)
#include <vector>      // for vector

namespace aoc::day11 {

class DeviceMap {
  public:
    using Key = std::uint16_t;

    static DeviceMap read(std::istream &is);
    long count_paths(const std::string_view source_name,
                     const std::string_view target_name) const;
    bool contains(const std::string_view name) const;

  private:
    Key lookup(const std::string &name);
    Key lookup(const std::string_view name) const;

    std::vector<std::string> names{};
    std::map<std::string, Key, std::less<>> name_lookup{};
    std::vector<std::set<Key>> neighbors{};
};

DeviceMap DeviceMap::read(std::istream &is) {
    DeviceMap device_map;
    // read file line-by-line
    std::string line;
    while (std::getline(is, line)) {
        std::istringstream iss{line};
        auto vec = aoc::read_vector<std::string>(iss);
        std::string source = vec.front();
        // remove trailing colon from name
        source.pop_back();
        auto src_key = device_map.lookup(source);
        for (std::size_t i = 1; i < vec.size(); ++i) {
            auto dst_key = device_map.lookup(vec[i]);
            device_map.neighbors[src_key].insert(dst_key);
        }
    }
    return device_map;
}

DeviceMap::Key DeviceMap::lookup(const std::string &name) {
    Key key = names.size();
    auto [it, inserted] = name_lookup.try_emplace(name, key);
    if (inserted) {
        // update names and neighbors
        names.emplace_back(it->first);
        neighbors.emplace_back();
        assert(names[key] == name);
        assert(name_lookup.at(name) == key);
        assert(neighbors.size() == name_lookup.size());
        assert(names.size() == name_lookup.size());
    }
    return it->second;
}

DeviceMap::Key DeviceMap::lookup(const std::string_view name) const {
    auto it = name_lookup.find(name);
    if (it == name_lookup.end()) {
        throw std::out_of_range("DeviceMap::lookup");
    }
    return it->second;
}

bool DeviceMap::contains(const std::string_view name) const {
    return name_lookup.contains(name);
}

long DeviceMap::count_paths(const std::string_view source_name,
                            const std::string_view target_name) const {
    // DP: topo sort, then iterate backwards
    Key source = lookup(source_name), target = lookup(target_name);

    const auto process_neighbors_ = [this](DeviceMap::Key key, auto &&process) {
        std::ranges::for_each(neighbors[key], process);
    };

    std::vector<Key> order = aoc::graph::topo_sort(source, process_neighbors_);
    assert(order.size() >= 2);
    assert(order[0] == source);

    std::vector<long> path_count(names.size());
    path_count[target] = 1;
    for (size_t i = order.size(); i-- > 0;) {
        Key u = order[i];
        for (Key v : neighbors[u]) {
            path_count[u] += path_count[v];
        }
    }
    if constexpr (aoc::DEBUG) {
        for (auto key : order) {
            std::cerr << names[key] << ": " << std::setw(3) << path_count[key]
                      << " ->";
            for (auto v : neighbors[key]) {
                std::cerr << " " << names[v];
            }
            std::cerr << "\n";
        }
    }

    return path_count[source];
}

} // namespace aoc::day11

#endif /* end of include guard: DAY11_HPP_0DCFGKPU */
