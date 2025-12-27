/******************************************************************************
 * File:        day08.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-12-08
 *****************************************************************************/

#include "day08.hpp"
#include "ds/bounded_heap.hpp"
#include "ds/disjoint_set.hpp"
#include "lib.hpp"
#include <cassert>    // for assert
#include <cmath>      // for sqrt
#include <cstddef>    // for size_t
#include <fstream>    // for ifstream
#include <functional> // for multiplies, greater
#include <iostream>   // for cout, cerr
#include <map>        // for map
#include <numeric>    // for reduce
#include <queue>      // for priority_queue
#include <set>        // for set
#include <vector>     // for vector

int main(int argc, char **argv) {
    auto args = aoc::parse_args(argc, argv);
    using namespace aoc::day08;
    using LongPos3 = aoc::LongPos3;
    std::vector<LongPos3> coords = read_input(args.infile);

    aoc::ds::disjoint_set dset;
    std::map<LongPos3, aoc::ds::disjoint_set::node_t *> ds_nodes{};

    std::priority_queue<JunctionBoxPair, std::vector<JunctionBoxPair>,
                        std::greater<JunctionBoxPair>>
        pqueue;
    for (std::size_t i = 0; i < coords.size(); ++i) {
        auto ds_node = dset.new_set();
        ds_nodes[coords[i]] = ds_node;
        // avoid duplicates by starting at i+1
        for (std::size_t j = i + 1; j < coords.size(); ++j) {
            pqueue.push({coords[i], coords[j]});
        }
    }

    const int part_1_count =
        args.input_type == aoc::InputType::EXAMPLE ? 10 : 1000;

    // use disjoint-set
    std::set<aoc::ds::disjoint_set::id_t> merged_ids{};
    for (int i = 0; i < part_1_count; ++i) {
        const JunctionBoxPair &jbp = pqueue.top();
        assert(jbp.dist_sq > 0);
        if constexpr (aoc::DEBUG) {
            std::cerr << std::sqrt(jbp.dist_sq) << ": " << jbp.box1 << " - "
                      << jbp.box2 << "\n";
        }
        auto node = ds_nodes.at(jbp.box1);
        if (dset.merge(node, ds_nodes.at(jbp.box2))) {
            merged_ids.insert(node->parent->id);
        }
        pqueue.pop();
    }
    // deduplicate based on root
    std::map<aoc::ds::disjoint_set::id_t, std::size_t> component_sizes{};
    for (const auto &pos : coords) {
        const auto *root = dset.find(ds_nodes.at(pos));
        component_sizes[root->id] = root->size;
    }

    // method from https://stackoverflow.com/a/2935995
    aoc::ds::bounded_heap<std::size_t, std::vector<std::size_t>,
                          std::greater<std::size_t>>
        largest_component_sizes(3);
    for (const auto &[_, size] : component_sizes) {
        largest_component_sizes.push(size);
    }
    if constexpr (aoc::DEBUG) {
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
    long part2 = 0;
    std::size_t prev_count = dset.set_count();
    while (dset.set_count() > 1) {
        const JunctionBoxPair &jbp = pqueue.top();
        dset.merge(ds_nodes.at(jbp.box1), ds_nodes.at(jbp.box2));
        std::size_t new_count = dset.set_count();
        if constexpr (aoc::DEBUG) {
            if (prev_count != new_count) {
                std::cerr << "# sets = " << new_count << "\n";
                prev_count = new_count;
            }
        } else {
            // suppress dead store warning
            (void)prev_count;
        }
        if (new_count == 1) {
            part2 = jbp.box1.x * jbp.box2.x;
        }
        pqueue.pop();
    }
    std::cout << part2 << "\n";

    return 0;
}
