/******************************************************************************
 * File:        day23.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-25
 *****************************************************************************/

#include "day23.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv).infile;

    auto graph = aoc::day23::ComputerGraph::read(infile);

    std::cout << graph.count_t_triangles() << "\n";

    return 0;
}
