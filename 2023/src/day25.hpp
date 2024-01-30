/******************************************************************************
 * File:        day25.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-26
 *****************************************************************************/

#ifndef DAY25_HPP_PXSWLG7Y
#define DAY25_HPP_PXSWLG7Y

#include "ds/pairing_heap.hpp" // for pairing_heap

#include "lib.hpp"       // for DEBUG
#include <cassert>       // for assert
#include <cstddef>       // for size_t
#include <iostream>      // for istream, cerr
#include <limits>        // for numeric_limits
#include <memory>        // for shared_ptr (pairing_heap)
#include <sstream>       // for istringstream
#include <string>        // for string, getline
#include <unordered_map> // for unordered_map
#include <unordered_set> // for unordered_set
#include <utility>       // for pair
// IWYU pragma: no_include <functional>  // for hash (unordered_{set,map})

namespace aoc::day25 {

using vertex_t = std::size_t;
using weight_t = unsigned int;
using cut_t = std::pair<weight_t, std::size_t>;

class Graph {
    std::unordered_map<vertex_t, std::unordered_map<vertex_t, weight_t>> edges;
    std::unordered_map<vertex_t, std::size_t> merged_counts;

  public:
    std::size_t num_vertices() const { return edges.size(); }
    void add_edge(vertex_t u, vertex_t v, weight_t weight = 1);

  private:
    void remove_vertex(vertex_t u);

    void merge_vertices(vertex_t u, vertex_t v);

  public:
    vertex_t get_arbitrary_vertex() const { return edges.begin()->first; }
    cut_t MinimumCutPhase(vertex_t a);
};

void Graph::add_edge(vertex_t u, vertex_t v, weight_t weight) {
    assert(u != v);
    bool inserted;
    inserted = edges[u].emplace(v, weight).second;
    assert(inserted);
    inserted = edges[v].emplace(u, weight).second;
    assert(inserted);
}

void Graph::remove_vertex(vertex_t u) {
    const auto &neighbors = edges.at(u);
    // remove incoming edges
    for (const auto &[v, _] : neighbors) {
        edges.at(v).erase(u);
    }
    // remove outgoing edges
    edges.erase(u);

    merged_counts.erase(u);
}

void Graph::merge_vertices(vertex_t s, vertex_t t) {
    // merge vertex t into s
    auto &s_neighbors = edges.at(s);
    auto &t_neighbors = edges.at(t);

    for (const auto &[v, w] : t_neighbors) {
        if (v == s) {
            continue;
        }
        auto it = s_neighbors.find(v);
        if (it != s_neighbors.end()) {
            it->second += w;
            edges.at(v).at(s) += w;
        } else {
            add_edge(s, v, w);
        }
    }
    merged_counts[s] = (merged_counts[s] + 1) + (merged_counts[t] + 1) - 1;
    remove_vertex(t);
}

// Stoer-Wagner minimum cut algorithm

cut_t Graph::MinimumCutPhase(vertex_t a) {
    std::unordered_set<vertex_t> A = {a};
    vertex_t s = a, t = a;
    weight_t cut_value = 0;
    aoc::ds::pairing_heap<std::pair<weight_t, vertex_t>> pq;
    std::unordered_map<vertex_t, decltype(pq)::handle_type> handle_map;
    for (const auto &[v, w] : edges.at(a)) {
        handle_map.emplace(v, pq.emplace(w, v));
    }
    while (A.size() != num_vertices()) {
        // add to A the most tightly connected vertex
        if constexpr (aoc::DEBUG) {
            std::cerr << "A: " << A.size() << "; pq: " << pq.size()
                      << "; handle_map: " << handle_map.size() << "\n";
        }
        const auto tmp = pq.top();
        pq.pop();
        handle_map.erase(tmp.second);
        s = t;
        t = tmp.second;
        cut_value = tmp.first;
        A.insert(t);
        // update or add all nodes connected to t in the priority queue
        for (const auto &[v, w] : edges.at(t)) {
            if (A.contains(v)) {
                continue;
            }
            auto it = handle_map.find(v);
            if (it != handle_map.end()) {
                weight_t prev_weight = it->second->value().first;
                pq.update(it->second, std::make_pair(prev_weight + w, v));
            } else {
                handle_map.emplace(v, pq.emplace(w, v));
            }
        }
    }
    cut_t cut_of_the_phase{cut_value, merged_counts[t] + 1};
    merge_vertices(s, t);
    return cut_of_the_phase;
}

cut_t MinimumCut(Graph G, vertex_t a) {
    cut_t min_cut = {std::numeric_limits<weight_t>::max(), {}};
    std::size_t total_iters = G.num_vertices() - 1;
    for (std::size_t i = 1; i <= total_iters; ++i) {
        cut_t cut_of_the_phase = G.MinimumCutPhase(a);
        if (cut_of_the_phase.first < min_cut.first) {
            min_cut = cut_of_the_phase;
        }
    }
    assert(G.num_vertices() == 1);
    return min_cut;
}

Graph read_input(std::istream &is) {
    Graph G;
    std::string line;
    std::unordered_map<std::string, vertex_t> vertex_names;
    const auto get_vertex =
        [&vertex_names](const std::string &name) -> vertex_t {
        const auto it = vertex_names.find(name);
        if (it == vertex_names.end()) {
            vertex_t id = vertex_names.size();
            vertex_names.emplace(name, id);
            return id;
        }
        return it->second;
    };
    while (std::getline(is, line)) {
        std::istringstream ss{line};
        std::string u;
        // read "name:"
        ss >> u;
        // strip trailing colon
        u.pop_back();
        vertex_t u_id = get_vertex(u);
        std::string v;
        while (ss >> v) {
            G.add_edge(u_id, get_vertex(v));
        }
    }
    return G;
}

} // namespace aoc::day25

#endif /* end of include guard: DAY25_HPP_PXSWLG7Y */
