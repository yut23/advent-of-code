/******************************************************************************
 * File:        graph_traversal.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-17
 * Description: Graph traversal algorithms for Advent of Code.
 *              Ported from AoC 2019 in Python.
 *****************************************************************************/

#ifndef GRAPH_TRAVERSAL_HPP_56T9ZURK
#define GRAPH_TRAVERSAL_HPP_56T9ZURK

#include <algorithm>     // for reverse
#include <concepts>      // for convertible_to
#include <cstddef>       // for size_t
#include <functional>    // for function, greater, hash
#include <map>           // for map
#include <optional>      // for optional
#include <queue>         // for priority_queue
#include <set>           // for set
#include <stdexcept>     // for invalid_argument
#include <type_traits>   // for conditional_t
#include <unordered_map> // for unordered_map
#include <unordered_set> // for unordered_set
#include <utility>       // for move, pair
#include <vector>        // for vector

/// graph traversal algorithms
namespace aoc::graph {

namespace detail {
// Declaration of the concept "Hashable", which is satisfied by any type 'T'
// such that for values 'a' of type 'T', the expression std::hash<T>{}(a)
// compiles and its result is convertible to std::size_t
template <typename T>
concept Hashable = requires(T a) {
                       {
                           std::hash<T>{}(a)
                           } -> std::convertible_to<std::size_t>;
                   };

template <class Key>
using maybe_unordered_set =
    std::conditional_t<Hashable<Key>, std::unordered_set<Key>, std::set<Key>>;

template <class Key, class T>
using maybe_unordered_map =
    std::conditional_t<Hashable<Key>, std::unordered_map<Key, T>,
                       std::map<Key, T>>;
} // namespace detail

/**
 * Generic BFS on an arbitrary graph.
 *
 * At least one of `is_target` and `visit` must be passed.
 *
 * Will visit each node no more than once, calling `visit` with the node and the
 * depth at which it was found.
 *
 * Returns the distance from the source to the first target found, or -1 if not
 * found.
 */
template <class Key>
int bfs(const Key &source,
        std::function<std::vector<Key>(const Key &)> get_neighbors,
        std::optional<std::function<bool(const Key &)>> is_target = {},
        std::optional<std::function<void(const Key &, int)>> visit = {}) {
    if (!is_target.has_value() && !visit.has_value()) {
        throw std::invalid_argument(
            "At least one of is_target and visit must be passed!");
    }
    int distance = 0;
    detail::maybe_unordered_set<Key> queue = {source};
    detail::maybe_unordered_set<Key> next_queue{};

    while (!queue.empty()) {
        for (const Key &key : queue) {
            if (visit.has_value()) {
                visit.value()(key, distance);
            }
            if (is_target.has_value() && is_target.value()(key)) {
                return distance;
            }
            auto neighbors = get_neighbors(key);
            next_queue.insert(neighbors.begin(), neighbors.end());
        }
        std::swap(queue, next_queue);
        ++distance;
    }
    return -1;
}

/**
 * Generic DFS on an arbitrary graph.
 *
 * At least one of `is_target` and `visit` must be passed.
 *
 * Will visit each node no more than once, calling `visit` with the node and the
 * depth at which it was found.
 *
 * Returns the distance from the source to the first target found, or -1 if not
 * found.
 */
template <class Key>
int dfs(const Key &source,
        std::function<std::vector<Key>(const Key &)> get_neighbors,
        std::optional<std::function<bool(const Key &)>> is_target = {},
        std::optional<std::function<void(const Key &, int)>> visit = {}) {
    if (!is_target.has_value() && !visit.has_value()) {
        throw std::invalid_argument(
            "At least one of is_target and visit must be passed!");
    }
    detail::maybe_unordered_set<Key> seen{};

    auto helper = [&seen, &get_neighbors, &is_target,
                   &visit](const Key &key, int depth, const auto &rec) {
        if (visit.has_value()) {
            visit.value()(key, depth);
        }
        if (is_target.has_value() && is_target.value()(key)) {
            return depth;
        }
        seen.insert(key);
        for (const Key &neighbor : get_neighbors(key)) {
            if (seen.contains(neighbor)) {
                continue;
            }
            int result = rec(neighbor, depth + 1, rec);
            if (result != -1) {
                return result;
            }
        }
        return -1;
    };

    return helper(source, 0, helper);
}

/**
 * Generic Dijkstra's algorithm on an arbitrary weighted graph.
 *
 * Returns the distance from the source to the first target found, or -1 if not
 * found.
 */
template <class Key>
std::pair<int, std::vector<Key>>
dijkstra(const Key &source,
         std::function<std::vector<Key>(const Key &)> get_neighbors,
         std::function<int(const Key &, const Key &)> get_distance,
         std::function<bool(const Key &)> is_target,
         std::optional<std::function<void(const Key &, int)>> visit = {}) {
    detail::maybe_unordered_set<Key> visited{};
    detail::maybe_unordered_map<Key, int> distances{};
    detail::maybe_unordered_map<Key, Key> prev{};

    using pq_key = std::pair<int, Key>;
    std::priority_queue<pq_key, std::vector<pq_key>, std::greater<pq_key>> pq{};

    distances[source] = 0;
    pq.emplace(0, source);

    while (!pq.empty()) {
        auto [dist, current] = std::move(pq.top());
        pq.pop();
        if (dist != distances[current]) {
            continue;
        }
        if (visit.has_value()) {
            visit.value()(current, dist);
        }
        if (is_target(current)) {
            std::vector<Key> path{current};
            const Key *u = &current;
            typename decltype(prev)::const_iterator it;
            while (*u != source && (it = prev.find(*u)) != prev.end()) {
                path.emplace_back(it->second);
                u = &it->second;
            }
            std::ranges::reverse(path);
            return {dist, path};
        }
        for (const Key &neighbor : get_neighbors(current)) {
            if (visited.contains(neighbor)) {
                continue;
            }
            int new_distance =
                distances[current] + get_distance(current, neighbor);
            auto it = distances.find(neighbor);
            if (it == distances.end() || new_distance < it->second) {
                distances[neighbor] = new_distance;
                prev[neighbor] = current;
                pq.emplace(new_distance, neighbor);
            }
        }
        visited.insert(std::move(current));
    }
    return {-1, {}};
}

/**
 * Shortest distances from a single node using Dijkstra's algorithm.
 */
template <class Key>
detail::maybe_unordered_map<Key, int>
shortest_distances(const Key &source,
                   std::function<std::vector<Key>(const Key &)> get_neighbors,
                   std::function<int(const Key &, const Key &)> get_distance) {
    detail::maybe_unordered_set<Key> visited{};
    detail::maybe_unordered_map<Key, int> distances{};
    std::priority_queue<std::pair<int, Key>> pq{};
    distances[source] = 0;
    pq.emplace(0, source);

    while (!pq.empty()) {
        auto [dist, current] = pq.top();
        pq.pop();
        if (dist != distances[current]) {
            continue;
        }
        for (const Key &neighbor : get_neighbors(current)) {
            if (visited.contains(neighbor)) {
                continue;
            }
            int new_distance =
                distances[current] + get_distance(current, neighbor);
            auto it = distances.find(neighbor);
            if (it == distances.end() || it->second > new_distance) {
                distances[neighbor] = new_distance;
                pq.emplace(new_distance, neighbor);
            }
        }
        visited.insert(std::move(current));
    }
    return distances;
}

// instantiate templates in an anonymous namespace, so the static analyzers will
// check these functions
namespace {
using Key1 = std::pair<int, int>;
[[maybe_unused]] void _lint_helper_unhashable(
    const Key1 &source,
    std::function<std::vector<Key1>(const Key1 &)> get_neighbors,
    std::function<bool(const Key1 &)> is_target,
    std::function<void(const Key1 &, int)> visit,
    std::function<int(const Key1 &, const Key1 &)> get_distance) {
    bfs<Key1>(source, get_neighbors);
    bfs<Key1>(source, get_neighbors, is_target);
    bfs<Key1>(source, get_neighbors, {}, visit);
    bfs<Key1>(source, get_neighbors, is_target, visit);

    dfs<Key1>(source, get_neighbors);
    dfs<Key1>(source, get_neighbors, is_target);
    dfs<Key1>(source, get_neighbors, {}, visit);
    dfs<Key1>(source, get_neighbors, is_target, visit);

    dijkstra<Key1>(source, get_neighbors, get_distance, is_target);
    dijkstra<Key1>(source, get_neighbors, get_distance, is_target, visit);

    shortest_distances<Key1>(source, get_neighbors, get_distance);
}
using Key2 = int;
[[maybe_unused]] void _lint_helper_hashable(
    const Key2 &source,
    std::function<std::vector<Key2>(const Key2 &)> get_neighbors,
    std::function<bool(const Key2 &)> is_target,
    std::function<void(const Key2 &, int)> visit,
    std::function<int(const Key2 &, const Key2 &)> get_distance) {
    bfs<Key2>(source, get_neighbors);
    bfs<Key2>(source, get_neighbors, is_target);
    bfs<Key2>(source, get_neighbors, {}, visit);
    bfs<Key2>(source, get_neighbors, is_target, visit);

    dfs<Key2>(source, get_neighbors);
    dfs<Key2>(source, get_neighbors, is_target);
    dfs<Key2>(source, get_neighbors, {}, visit);
    dfs<Key2>(source, get_neighbors, is_target, visit);

    dijkstra<Key2>(source, get_neighbors, get_distance, is_target);
    dijkstra<Key2>(source, get_neighbors, get_distance, is_target, visit);

    shortest_distances<Key2>(source, get_neighbors, get_distance);
}
} // namespace

} // namespace aoc::graph

#endif /* end of include guard: GRAPH_TRAVERSAL_HPP_56T9ZURK */
