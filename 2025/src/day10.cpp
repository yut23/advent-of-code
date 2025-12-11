/******************************************************************************
 * File:        day10.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-12-10
 *****************************************************************************/

#include "day10.hpp"
#include "graph_traversal.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv).infile;
    using namespace aoc::day10;

    auto machines = read_input(infile);

    int part1 = 0;
    for (const Machine &machine : machines) {
        // generalized BFS!
        std::initializer_list<BfsEntry> sources{BfsEntry(machine)};
        const auto process_neighbors = [](const BfsEntry &entry,
                                          const auto &process) {
            BfsEntry::process_neighbors(entry, process);
        };
        part1 += aoc::graph::bfs(sources, process_neighbors,
                                 BfsEntry::is_target, {});
    }
    std::cout << part1 << "\n";

    return 0;
}
