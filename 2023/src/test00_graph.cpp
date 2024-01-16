/******************************************************************************
 * File:        test00_graph.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-01-15
 * Description: Unit tests for graph algorithms.
 *****************************************************************************/

#include "unit_test/unit_test.hpp"

#include "graph_traversal.hpp"
#include <cstddef>       // for size_t
#include <unordered_map> // for unordered_map
#include <unordered_set> // for unordered_set
#include <vector>        // for vector
// IWYU pragma: no_include <algorithm>  // for copy

namespace aoc::graph::test {

std::size_t test_tarjan_scc() {
    unit_test::PureTest test(
        "aoc::graph::tarjan_scc",
        +[](const std::unordered_map<int, std::vector<int>> &adj,
            int root) -> std::vector<std::unordered_set<int>> {
            const auto get_neighbors = [&adj](int key) -> std::vector<int> {
                return adj.at(key);
            };
            return aoc::graph::tarjan_scc(root, get_neighbors);
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
         1, {{1, 2, 8}, {3, 4, 5, 7}, {6}});
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
         1, {{1}, {3, 5}, {2, 4}, {6, 7, 8}});

    return test.done(), test.num_failed();
}

} // namespace aoc::graph::test

int main() {
    std::size_t failed_count = 0;
    failed_count += aoc::graph::test::test_tarjan_scc();
    return unit_test::fix_exit_code(failed_count);
}
