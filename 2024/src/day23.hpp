/******************************************************************************
 * File:        day23.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-25
 *****************************************************************************/

#ifndef DAY23_HPP_H1FPOEBL
#define DAY23_HPP_H1FPOEBL

#include "lib.hpp"       // for DEBUG
#include <algorithm>     // for ranges::sort
#include <iostream>      // for istream, cerr
#include <string>        // for string
#include <unordered_map> // for unordered_map
#include <unordered_set> // for unordered_set
#include <vector>        // for vector

namespace aoc::day23 {

class ComputerGraph {
    std::unordered_map<std::string, std::unordered_set<std::string>> edges;

    void add_edge(std::string &&u, std::string &&v) {
        edges[u].insert(v);
        edges[std::move(v)].insert(std::move(u));
    }

    void remove_vertex(const std::string &v) {
        // remove from neighbors' edges
        for (const auto &u : edges.at(v)) {
            edges.at(u).erase(v);
        }
        // remove from edges
        edges.erase(v);
    }

  public:
    int count_t_triangles();
    static ComputerGraph read(std::istream &is);
};

/**
 * Triangle-finding algorithm from Chiba & Nishizeki, 1985
 * (doi:10.1137/0214017).
 */
int ComputerGraph::count_t_triangles() {
    // sort vertices from highest degree to lowest degree
    std::vector<std::string> vertices;
    for (const auto &[v, _] : edges) {
        vertices.push_back(v);
    }
    std::ranges::sort(vertices, std::greater{}, [this](const std::string &v) {
        return this->edges.at(v).size();
    });
    if constexpr (aoc::DEBUG) {
        std::cerr << "number of vertices: " << edges.size() << "\n";
        std::cerr << "vertex degrees, from highest to lowest:\n";
        for (const auto &v : vertices) {
            std::cerr << "  " << v << ": " << edges.at(v).size() << "\n";
        }
    }

    int triangle_count = 0;
    for (const std::string &v : vertices) {
        // mark all neighbors of v
        std::unordered_set<std::string> marks = edges.at(v);
        // check all edges connected to neighbors of v, and output a triangle
        // if both ends are marked
        for (const auto &u : marks) {
            for (const auto &w : edges.at(u)) {
                if (u < w && marks.contains(w) &&
                    (v[0] == 't' || u[0] == 't' || w[0] == 't')) {
                    ++triangle_count;
                }
            }
        }
        remove_vertex(v);
    }
    return triangle_count;
}

ComputerGraph ComputerGraph::read(std::istream &is) {
    std::string line;
    ComputerGraph graph;
    while (is >> line) {
        graph.add_edge(line.substr(0, 2), line.substr(3, 2));
    }
    return graph;
}

} // namespace aoc::day23

#endif /* end of include guard: DAY23_HPP_H1FPOEBL */
