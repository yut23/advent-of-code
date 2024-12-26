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

    const auto password_parts = graph.find_password();
    // this modifies the graph, so do it second
    std::cout << graph.count_t_triangles() << "\n";

    for (auto it = password_parts.begin(); it != password_parts.end(); ++it) {
        if (it != password_parts.begin()) {
            std::cout << ",";
        }
        std::cout << *it;
    }
    std::cout << "\n";

    return 0;
}
