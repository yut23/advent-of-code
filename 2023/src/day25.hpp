/******************************************************************************
 * File:        day25.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-26
 *****************************************************************************/

#ifndef DAY25_HPP_PXSWLG7Y
#define DAY25_HPP_PXSWLG7Y

#include "lib.hpp"       // for DEBUG
#include <cassert>       // for assert
#include <cstddef>       // for size_t
#include <iostream>      // for istream, cerr
#include <limits>        // for numeric_limits
#include <queue>         // for priority_queue
#include <sstream>       // for istringstream
#include <string>        // for string, getline
#include <unordered_map> // for unordered_map
#include <utility>       // for pair
#include <vector>        // for vector
// IWYU pragma: no_include <algorithm>  // for fill_n (vector)
// IWYU pragma: no_include <functional>  // for hash (unordered_map)

namespace aoc::day25 {

using vertex_t = unsigned int;
using weight_t = unsigned int;
using cut_t = std::pair<weight_t, std::size_t>;

class Graph {
    std::vector<std::unordered_map<vertex_t, weight_t>> edges;
    std::unordered_map<vertex_t, std::size_t> merged_counts;
    std::size_t m_vertex_count = 0;
    std::size_t m_next_vertex = 0;

  public:
    std::size_t num_vertices() const { return m_vertex_count; }
    std::size_t next_vertex() const { return m_next_vertex; }
    void add_edge(vertex_t u, vertex_t v, weight_t weight = 1,
                  bool allow_resize = true);

  private:
    void remove_vertex(vertex_t u);

    void merge_vertices(vertex_t u, vertex_t v);

  public:
    vertex_t get_arbitrary_vertex() const { return 0; }
    cut_t MinimumCutPhase(vertex_t a);
};

void Graph::add_edge(vertex_t u, vertex_t v, weight_t weight,
                     bool allow_resize) {
    assert(u != v);
    if (u > v) {
        std::swap(u, v);
    }
    // v is now the larger vertex
    if (allow_resize && v >= m_next_vertex) {
        m_next_vertex = v + 1;
        edges.resize(m_next_vertex);
        m_vertex_count = m_next_vertex;
    }
    edges[u].emplace(v, weight);
    edges[v].emplace(u, weight);
}

void Graph::remove_vertex(vertex_t u) {
    auto &neighbors = edges[u];
    // remove incoming edges
    for (const auto &[v, _] : neighbors) {
        edges[v].erase(u);
    }
    // remove outgoing edges
    neighbors.clear();

    merged_counts.erase(u);
    --m_vertex_count;
}

void Graph::merge_vertices(vertex_t s, vertex_t t) {
    // merge vertex t into s
    auto &s_neighbors = edges[s];
    auto &t_neighbors = edges[t];

    for (const auto &[v, w] : t_neighbors) {
        if (v == s) {
            continue;
        }
        auto it = s_neighbors.find(v);
        if (it != s_neighbors.end()) {
            it->second += w;
            edges[v].at(s) += w;
        } else {
            const bool allow_resize = false;
            add_edge(s, v, w, allow_resize);
        }
    }
    merged_counts[s] = (merged_counts[s] + 1) + (merged_counts[t] + 1) - 1;
    remove_vertex(t);
}

// Stoer-Wagner minimum cut algorithm

cut_t Graph::MinimumCutPhase(vertex_t a) {
    std::vector<bool> A(next_vertex(), false);
    A[a] = true;
    std::size_t A_count = 1;
    vertex_t s = a, t = a;
    weight_t cut_value = 0;
    std::priority_queue<std::pair<weight_t, vertex_t>> pq;
    std::vector<weight_t> weight_sum(next_vertex(), 0);
    for (const auto &[v, w] : edges[a]) {
        pq.emplace(w, v);
        weight_sum[v] = w;
    }
    while (A_count != num_vertices()) {
        // add to A the most tightly connected vertex
        if constexpr (aoc::DEBUG) {
            std::cerr << "A: " << A_count << "; pq: " << pq.size()
                      << "; num_vertices: " << num_vertices() << "\n";
        }
        const auto tmp = pq.top();
        pq.pop();
        if (A[tmp.second]) {
            continue;
        }
        s = t;
        t = tmp.second;
        cut_value = tmp.first;
        A[t] = true;
        ++A_count;
        // update or add all nodes connected to t in the priority queue
        for (const auto &[v, w] : edges[t]) {
            if (A[v]) {
                continue;
            }
            auto &weight = weight_sum[v];
            weight += w;
            pq.emplace(weight, v);
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
