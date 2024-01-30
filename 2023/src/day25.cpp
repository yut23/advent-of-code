/******************************************************************************
 * File:        day25.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-26
 *****************************************************************************/

#include "day25.hpp"
#include "lib.hpp"  // for parse_args
#include <cassert>  // for assert
#include <iostream> // for cout

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    using namespace aoc::day25;
    Graph G = read_input(infile);

    const auto &[wire_count, cut_group_size] =
        MinimumCut(G, G.get_arbitrary_vertex());
    assert(wire_count == 3);
    std::size_t part_1 = cut_group_size * (G.num_vertices() - cut_group_size);
    std::cout << part_1 << "\n";

    return 0;
}
