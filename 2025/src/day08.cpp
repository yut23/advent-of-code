/******************************************************************************
 * File:        day08.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-12-08
 *****************************************************************************/

#include "day08.hpp"
#include "ds/bounded_heap.hpp"
#include "graph_traversal.hpp" // for tarjan_scc
#include "lib.hpp"
#include "unit_test/pretty_print.hpp"
#include <algorithm>     // for for_each
#include <cassert>       // for assert
#include <cmath>         // for sqrt
#include <cstddef>       // for size_t
#include <fstream>       // for ifstream
#include <functional>    // for multiplies, hash
#include <iostream>      // for cout, cerr
#include <numeric>       // for reduce
#include <set>           // for set (tarjan_scc)
#include <unordered_map> // for unordered_map
#include <vector>        // for vector

int main(int argc, char **argv) {
    auto args = aoc::parse_args(argc, argv);
    using namespace aoc::day08;
    using LongPos3 = aoc::LongPos3;
    std::vector<LongPos3> coords = read_input(args.infile);

    // method from https://stackoverflow.com/a/2935995
    aoc::ds::bounded_heap<JunctionBoxPair> pair_heap(
        args.input_type == aoc::InputType::EXAMPLE ? 10 : 1000);
    for (std::size_t i = 0; i < coords.size(); ++i) {
        // avoid duplicates by starting at i+1
        for (std::size_t j = i + 1; j < coords.size(); ++j) {
            pair_heap.push({coords[i], coords[j]});
        }
    }

    if constexpr (aoc::DEBUG) {
        std::cerr << "pair_heap.size() = " << pair_heap.size() << "\n";
        for (const auto &jbp : pair_heap) {
            std::cerr << std::sqrt(jbp.dist_sq) << ": " << jbp.box1 << " - "
                      << jbp.box2 << "\n";
        }
    }

    // use disjoint-set or tarjan_scc
    std::unordered_map<LongPos3, std::vector<LongPos3>> neighbors{};
    for (const auto &jbp : pair_heap) {
        neighbors[jbp.box1].push_back(jbp.box2);
        neighbors[jbp.box2].push_back(jbp.box1);
    }
    auto components =
        aoc::graph::tarjan_scc(coords, [&](const LongPos3 &key,
                                           auto &&process) {
            if (auto it = neighbors.find(key); it != neighbors.end()) {
                std::ranges::for_each(it->second, process);
            }
        }).first;

    aoc::ds::bounded_heap<std::size_t, std::vector<std::size_t>,
                          std::greater<std::size_t>>
        largest_component_sizes(3);
    for (const auto &component : components) {
        largest_component_sizes.push(component.size());
    }
    if constexpr (aoc::DEBUG) {
        std::cerr << "# components = " << components.size() << "\n";
        for (const auto &component : components) {
            std::cerr << pretty_print::repr(component) << "\n";
        }
        std::cerr << largest_component_sizes.size()
                  << " largest component sizes:";
        for (auto x : largest_component_sizes) {
            std::cerr << " " << x;
        }
        std::cerr << "\n";
    }
    std::size_t part1 =
        std::reduce(largest_component_sizes.begin(),
                    largest_component_sizes.end(), 1, std::multiplies<>());
    std::cout << part1 << "\n";
    if (args.input_type == aoc::InputType::MAIN) {
        assert(part1 > 13024);
    }

    return 0;
}
