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
#include <functional>    // for multiplies, hash, greater
#include <iostream>      // for cout, cerr
#include <numeric>       // for reduce
#include <queue>         // for priority_queue
#include <set>           // for set (tarjan_scc)
#include <unordered_map> // for unordered_map
#include <vector>        // for vector

int main(int argc, char **argv) {
    auto args = aoc::parse_args(argc, argv);
    using namespace aoc::day08;
    using LongPos3 = aoc::LongPos3;
    std::vector<LongPos3> coords = read_input(args.infile);

    std::priority_queue<JunctionBoxPair, std::vector<JunctionBoxPair>,
                        std::greater<JunctionBoxPair>>
        pqueue;
    for (std::size_t i = 0; i < coords.size(); ++i) {
        // avoid duplicates by starting at i+1
        for (std::size_t j = i + 1; j < coords.size(); ++j) {
            pqueue.push({coords[i], coords[j]});
        }
    }

    const int part_1_count =
        args.input_type == aoc::InputType::EXAMPLE ? 10 : 1000;

    // use disjoint-set or tarjan_scc
    std::unordered_map<LongPos3, std::vector<LongPos3>> neighbors{};
    for (int i = 0; i < part_1_count; ++i) {
        const JunctionBoxPair &jbp = pqueue.top();
        assert(jbp.dist_sq > 0);
        if constexpr (aoc::DEBUG) {
            std::cerr << std::sqrt(jbp.dist_sq) << ": " << jbp.box1 << " - "
                      << jbp.box2 << "\n";
        }
        neighbors[jbp.box1].push_back(jbp.box2);
        neighbors[jbp.box2].push_back(jbp.box1);
        pqueue.pop();
    }
    const auto process_neighbors = [&](const LongPos3 &key, auto &&process) {
        if (auto it = neighbors.find(key); it != neighbors.end()) {
            std::ranges::for_each(it->second, process);
        }
    };
    auto components = aoc::graph::tarjan_scc(coords, process_neighbors).first;

    // method from https://stackoverflow.com/a/2935995
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

    // part 2
    // quick and dirty (actually slow and dirty, 1.7s in fast mode, 8.3s in
    // release mode)
    long part2 = 0;
    std::size_t prev_num_components = components.size();
    while (components.size() > 1) {
        const JunctionBoxPair &jbp = pqueue.top();
        neighbors[jbp.box1].push_back(jbp.box2);
        neighbors[jbp.box2].push_back(jbp.box1);
        components = aoc::graph::tarjan_scc(coords, process_neighbors).first;
        if constexpr (aoc::DEBUG) {
            if (prev_num_components != components.size()) {
                std::cerr << "# components = " << components.size() << "\n";
                for (const auto &component : components) {
                    std::cerr << pretty_print::repr(component) << "\n";
                }
                prev_num_components = components.size();
            }
        } else {
            // suppress dead store warning
            (void)prev_num_components;
        }
        if (components.size() == 1) {
            part2 = jbp.box1.x * jbp.box2.x;
        }
        pqueue.pop();
    }
    std::cout << part2 << "\n";

    return 0;
}
