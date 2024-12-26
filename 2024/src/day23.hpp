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
    std::vector<std::string> find_password() const;
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
            triangle_count += std::ranges::count_if(
                edges.at(u), [&marks, &v, &u](const std::string &w) {
                    return u < w &&
                           (v[0] == 't' || u[0] == 't' || w[0] == 't') &&
                           marks.contains(w);
                });
        }
        remove_vertex(v);
    }
    return triangle_count;
}

/**
 * Find maximum clique through brute force. Bounded by the observation that
 * every vertex in the full input has degree 13.
 */
std::vector<std::string> ComputerGraph::find_password() const {
    // repeatedly find a maximal clique using the greedy algorithm
    std::unordered_set<std::string> pending_vertices, current_clique,
        maximum_clique;

    for (const auto &[v, _] : edges) {
        pending_vertices.insert(v);
    }

    while (!pending_vertices.empty()) {
        current_clique.clear();
        // add an arbitrary vertex to the current clique
        const std::string vertex = *pending_vertices.begin();
        current_clique.insert(vertex);
        pending_vertices.erase(vertex);
        // loop through the neighbors of this vertex, adding them to the clique
        // if they are adjacent to all vertices already in the clique
        for (const auto &u : edges.at(vertex)) {
            const auto &u_neighbors = edges.at(u);
            if (std::all_of(current_clique.begin(), current_clique.end(),
                            [&u_neighbors](const std::string &v) {
                                return u_neighbors.contains(v);
                            })) {
                // add u to the current clique
                current_clique.insert(u);
                // remove u from pending vertices
                pending_vertices.erase(u);
            }
        }

        if constexpr (aoc::DEBUG) {
            std::cerr << "found clique of size " << current_clique.size()
                      << ":\n";
            for (const std::string &u : current_clique) {
                std::cerr << " " << u;
            }
            std::cerr << "\n";
        }
        if (current_clique.size() > maximum_clique.size()) {
            if constexpr (aoc::DEBUG) {
                std::cerr << " new maximum!\n";
            }
            maximum_clique = current_clique;
        }
    }

    std::vector<std::string> password_parts(maximum_clique.begin(),
                                            maximum_clique.end());
    std::sort(password_parts.begin(), password_parts.end());
    return password_parts;
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
