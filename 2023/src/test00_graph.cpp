/******************************************************************************
 * File:        test00_graph.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-01-15
 * Description: Unit tests for graph algorithms.
 *****************************************************************************/

#include "unit_test/unit_test.hpp"

#include "graph_traversal.hpp"
#include <algorithm>     // for sort
#include <cstddef>       // for size_t
#include <set>           // for set
#include <unordered_map> // for unordered_map
#include <utility>       // for pair
#include <vector>        // for vector

namespace aoc::graph::test {

std::size_t test_tarjan_scc() {
    unit_test::PureTest test(
        "aoc::graph::tarjan_scc",
        +[](const std::unordered_map<int, std::vector<int>> &adj,
            int root) -> std::pair<std::vector<std::vector<int>>,
                                   std::set<std::pair<int, int>>> {
            const auto process_neighbors = [&adj](int key, auto &&visit) {
                std::ranges::for_each(adj.at(key), visit);
            };
            auto result = aoc::graph::tarjan_scc(root, process_neighbors);
            for (auto &component : result.first) {
                std::sort(component.begin(), component.end());
            }
            return result;
        });

    // example from Tarjan's original 1972 paper, "Depth-first search and
    // linear graph algorithms" (10.1137/0201010)
    test({{1, {2}},
          {2, {3, 8}},
          {3, {4, 7}},
          {4, {5}},
          {5, {3, 6}},
          {6, {}},
          {7, {4, 6}},
          {8, {1, 7}}},
         1, {{{1, 2, 8}, {3, 4, 5, 7}, {6}}, {{0, 1}, {1, 2}}});
    // example from Wikipedia:
    // https://commons.wikimedia.org/wiki/File:Tarjan%27s_Algorithm_Animation.gif
    // numbering:
    //  8 6 4 2
    //  7 5 3 1
    test({{1, {1, 2, 3}},
          {2, {4}},
          {3, {4, 5}},
          {4, {2, 6}},
          {5, {3, 6, 7}},
          {6, {8}},
          {7, {6}},
          {8, {7}}},
         1,
         {{{1}, {3, 5}, {2, 4}, {6, 7, 8}},
          {{0, 1}, {0, 2}, {1, 2}, {1, 3}, {2, 3}}});

    return test.done(), test.num_failed();
}

} // namespace aoc::graph::test

int main() {
    std::size_t failed_count = 0;
    failed_count += aoc::graph::test::test_tarjan_scc();
    return unit_test::fix_exit_code(failed_count);
}
