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

    // part 1 ends up clearing the graph, so do part 2 first
    const auto password_parts = graph.find_password();

    // part 1
    std::cout << graph.count_t_triangles() << "\n";

    // output the password for part 2
    for (auto it = password_parts.begin(); it != password_parts.end(); ++it) {
        if (it != password_parts.begin()) {
            std::cout << ",";
        }
        std::cout << *it;
    }
    std::cout << "\n";

    return 0;
}
