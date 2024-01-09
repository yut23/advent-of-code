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
#include <concepts>      // for convertible_to, same_as, integral
#include <cstddef>       // for size_t
#include <functional>    // for function, greater, hash
#include <iterator>      // for forward_iterator
#include <map>           // for map
#include <queue>         // for priority_queue
#include <set>           // for set
#include <stdexcept>     // for invalid_argument
#include <type_traits>   // for conditional_t // IWYU pragma: export
#include <unordered_map> // for unordered_map
#include <unordered_set> // for unordered_set
#include <utility>       // for move, pair, make_pair, swap, forward
#include <vector>        // for vector
// IWYU pragma: no_include <compare>  // for operator==

// the export above avoids having to include type_traits in all source files
// for invoke_result_t

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

template <typename C, typename T>
// clang-format off
concept any_iterable_collection =
    std::same_as<typename C::value_type, T> &&
    requires (C c) {
        { c.begin() } -> std::forward_iterator;
        { c.end() }   -> std::forward_iterator;
        { const_cast<const C&>(c).begin() } -> std::forward_iterator;
        { const_cast<const C&>(c).end() }   -> std::forward_iterator;
    };
// clang-format on

template <class Func, class Key>
concept GetNeighbors = requires(Func get_neighbors, const Key &key) {
                           {
                               get_neighbors(key)
                               } -> any_iterable_collection<Key>;
                       };

template <class Func, class Key>
concept IsTarget = requires(Func is_target, const Key &key) {
                       { is_target(key) } -> std::same_as<bool>;
                   };

template <class Func, class Key>
concept Visit = requires(Func visit, const Key &key, int distance) {
                    { visit(key, distance) } -> std::same_as<void>;
                };

template <class Func, class Key>
concept VisitWithParent = requires(Func visit_with_parent, const Key &key,
                                   const Key &parent, int depth) {
                              {
                                  visit_with_parent(key, parent, depth)
                                  } -> std::same_as<void>;
                          };

template <class Func, class Key>
concept GetDistance = requires(Func get_distance, const Key &u, const Key &v) {
                          { get_distance(u, v) } -> std::integral;
                      };

template <class Func, class Key>
concept Heuristic = requires(Func heuristic, const Key &key) {
                        { heuristic(key) } -> std::integral;
                    };

} // namespace detail

/**
 * Generic BFS on an arbitrary graph.
 *
 * At least one of `is_target` and `visit` must be passed.
 *
 * If passed, `visit(node, distance)` will be called for each node.

 * If `use_seen` is true, nodes will only be visited once (i.e. a DAG will be
 * visited as a tree). `use_seen` should only be set to false if the graph has
 * no cycles.
 *
 * Returns the distance from the source to the first target found, or -1 if not
 * found.
 */
template <bool use_seen = true, class Key,
          detail::GetNeighbors<Key> GetNeighbors,
          detail::IsTarget<Key> IsTarget, detail::Visit<Key> Visit>
int bfs(const Key &source, GetNeighbors &&get_neighbors, IsTarget &&is_target,
        Visit &&visit) {
    int distance = 0;
    detail::maybe_unordered_set<Key> queue = {source};
    detail::maybe_unordered_set<Key> next_queue{};
    detail::maybe_unordered_set<Key> seen{};

    while (!queue.empty()) {
        for (const Key &key : queue) {
            visit(key, distance);
            if (is_target(key)) {
                return distance;
            }
            if constexpr (use_seen) {
                seen.insert(key);
            }
            for (const Key &neighbor : get_neighbors(key)) {
                if constexpr (use_seen) {
                    if (seen.contains(neighbor)) {
                        continue;
                    }
                }
                next_queue.insert(neighbor);
            }
        }
        queue.clear();
        std::swap(queue, next_queue);
        ++distance;
    }
    return -1;
}

template <bool use_seen = true, class Key,
          detail::GetNeighbors<Key> GetNeighbors,
          detail::IsTarget<Key> IsTarget>
int bfs(const Key &source, GetNeighbors &&get_neighbors, IsTarget &&is_target) {
    return bfs<use_seen>(source, std::forward<GetNeighbors>(get_neighbors),
                         std::forward<IsTarget>(is_target),
                         [](const Key &, int) {});
}

template <bool use_seen = true, class Key,
          detail::GetNeighbors<Key> GetNeighbors, detail::Visit<Key> Visit>
int bfs(const Key &source, GetNeighbors &&get_neighbors, Visit &&visit) {
    return bfs<use_seen>(
        source, std::forward<GetNeighbors>(get_neighbors),
        [](const Key &) { return false; }, std::forward<Visit>(visit));
}

/**
 * Generic DFS on an arbitrary graph.
 *
 * At least one of `is_target` and `visit_with_parent` must be passed.
 *
 * If passed, `visit_with_parent(node, parent, depth)` will be called for each
 * node.
 *
 * If `use_seen` is true, nodes will only be visited once (i.e. a DAG will be
 * visited as a tree). `use_seen` should only be set to false if the graph has
 * no cycles.
 *
 * Returns the distance from the source to the first target found, or -1 if not
 * found.
 */
template <bool use_seen = true, class Key,
          detail::GetNeighbors<Key> GetNeighbors,
          detail::IsTarget<Key> IsTarget,
          detail::VisitWithParent<Key> VisitWithParent>
int dfs(const Key &source, GetNeighbors &&get_neighbors, IsTarget &&is_target,
        VisitWithParent &&visit_with_parent) {
    detail::maybe_unordered_set<Key> seen{};

    auto helper = [&seen, &get_neighbors, &is_target,
                   &visit_with_parent](const Key &key, int depth,
                                       const Key &parent, const auto &rec) {
        visit_with_parent(key, parent, depth);
        if (is_target(key)) {
            return depth;
        }
        if constexpr (use_seen) {
            seen.insert(key);
        } else {
            // suppress unused lambda capture warning
            (void)seen;
        }
        for (const Key &neighbor : get_neighbors(key)) {
            if constexpr (use_seen) {
                if (seen.contains(neighbor)) {
                    continue;
                }
            }
            int result = rec(neighbor, depth + 1, key, rec);
            if (result != -1) {
                return result;
            }
        }
        return -1;
    };

    return helper(source, 0, source, helper);
}

template <bool use_seen = true, class Key,
          detail::GetNeighbors<Key> GetNeighbors,
          detail::IsTarget<Key> IsTarget>
int dfs(const Key &source, GetNeighbors &&get_neighbors, IsTarget &&is_target) {
    return dfs<use_seen>(source, std::forward<GetNeighbors>(get_neighbors),
                         std::forward<IsTarget>(is_target),
                         [](const Key &, const Key &, int) {});
}

template <bool use_seen = true, class Key,
          detail::GetNeighbors<Key> GetNeighbors,
          detail::VisitWithParent<Key> VisitWithParent>
int dfs(const Key &source, GetNeighbors &&get_neighbors,
        VisitWithParent &&visit_with_parent) {
    return dfs<use_seen>(
        source, std::forward<GetNeighbors>(get_neighbors),
        [](const Key &) { return false; },
        std::forward<VisitWithParent>(visit_with_parent));
}

/**
 * Topologically sort a directed acyclic graph.
 *
 * Throws a std::invalid_argument exception if the graph contains any cycles.
 */
template <class Key, detail::GetNeighbors<Key> GetNeighbors>
std::vector<Key> topo_sort(const Key &source, GetNeighbors &&get_neighbors) {
    detail::maybe_unordered_set<Key> temp_marks{};
    detail::maybe_unordered_set<Key> perm_marks{};

    std::vector<Key> ordered{};

    auto visit = [&](const Key &n, auto &rec) -> void {
        if (perm_marks.contains(n)) {
            return;
        }
        if (temp_marks.contains(n)) {
            throw std::invalid_argument("graph contains at least one cycle");
        }
        temp_marks.emplace(n);
        for (const Key &neighbor : get_neighbors(n)) {
            rec(neighbor, rec);
        }
        perm_marks.insert(temp_marks.extract(n));
        ordered.emplace_back(n);
    };

    visit(source, visit);

    std::ranges::reverse(ordered);

    return ordered;
}

/**
 * Longest path algorithm for a DAG.
 */
template <class Key, detail::GetNeighbors<Key> GetNeighbors,
          detail::GetDistance<Key> GetDistance, detail::IsTarget<Key> IsTarget>
std::pair<int, std::vector<Key>>
longest_path_dag(const Key &source, GetNeighbors &&get_neighbors,
                 GetDistance &&get_distance, IsTarget &&is_target) {
    std::vector<Key> ordering = topo_sort<Key>(source, get_neighbors);
    detail::maybe_unordered_map<Key, std::pair<int, Key>> longest_path{};
    // longest_path.try_emplace(source, std::make_pair(0, source));

    // find all the incoming neighbors of each node
    detail::maybe_unordered_map<Key, detail::maybe_unordered_set<Key>>
        incoming_neighbors{};
    incoming_neighbors[source] = {};
    constexpr bool use_seen = false;
    dfs<use_seen>(
        source, get_neighbors,
        [&incoming_neighbors](const Key &node, const Key &parent, int) {
            incoming_neighbors[node].emplace(parent);
        });

    // find longest path from source to each node
    longest_path.emplace(source, std::make_pair(0, source));
    for (const auto &key : ordering) {
        std::pair<int, Key> max_distance = {0, key};
        for (const auto &parent : incoming_neighbors.at(key)) {
            int new_distance = get_distance(parent, key);
            const auto it = longest_path.find(parent);
            if (it != longest_path.end()) {
                new_distance += longest_path.at(parent).first;
            }
            if (new_distance > max_distance.first) {
                max_distance = {new_distance, parent};
            }
        }
        longest_path.try_emplace(key, std::move(max_distance));
    }

    // find furthest target from source
    int max_distance = 0;
    Key target = source;
    for (const auto &[key, dist_pair] : longest_path) {
        const int &distance = dist_pair.first;
        if (is_target(key) && distance > max_distance) {
            max_distance = distance;
            target = key;
        }
    }

    // reconstruct path from source to target
    std::vector<Key> path{target};
    typename decltype(longest_path)::const_iterator it;
    while (path.back() != source &&
           (it = longest_path.find(path.back())) != longest_path.end()) {
        path.emplace_back(it->second.second);
    }
    std::ranges::reverse(path);
    return {max_distance, path};
}

/**
 * Generic Dijkstra's algorithm on an arbitrary weighted graph.
 *
 * Returns the distance and path from the source to the first target found,
 * or -1 and an empty path if not found.
 */
template <bool use_visited = false, class Key,
          detail::GetNeighbors<Key> GetNeighbors,
          detail::GetDistance<Key> GetDistance, detail::IsTarget<Key> IsTarget,
          detail::Visit<Key> Visit>
std::pair<int, std::vector<Key>>
dijkstra(const Key &source, GetNeighbors &&get_neighbors,
         GetDistance &&get_distance, IsTarget &&is_target, Visit &&visit) {
    detail::maybe_unordered_set<Key> visited{};
    detail::maybe_unordered_map<Key, std::pair<int, Key>> distances{};

    using pq_key = std::pair<int, Key>;
    std::priority_queue<pq_key, std::vector<pq_key>, std::greater<pq_key>>
        frontier{};

    distances[source] = {0, source};
    frontier.emplace(0, source);

    while (!frontier.empty()) {
        auto [dist, current] = std::move(frontier.top());
        frontier.pop();
        if (dist != distances.at(current).first) {
            continue;
        }
        visit(current, dist);
        if (is_target(current)) {
            // reconstruct path
            std::vector<Key> path{current};
            typename decltype(distances)::const_iterator it;
            while (path.back() != source &&
                   (it = distances.find(path.back())) != distances.end()) {
                path.emplace_back(it->second.second);
            }
            std::ranges::reverse(path);
            return {dist, path};
        }
        for (const Key &neighbor : get_neighbors(current)) {
            if constexpr (use_visited) {
                if (visited.contains(neighbor)) {
                    continue;
                }
            }
            int new_distance = dist + get_distance(current, neighbor);
            auto it = distances.find(neighbor);
            if (it == distances.end() || new_distance < it->second.first) {
                std::pair<int, Key> value{new_distance, current};
                if (it != distances.end()) {
                    it->second = std::move(value);
                } else {
                    distances.try_emplace(neighbor, std::move(value));
                }
                frontier.emplace(new_distance, neighbor);
            }
        }
        if constexpr (use_visited) {
            visited.insert(std::move(current));
        }
    }
    return {-1, {}};
}

template <bool use_visited = false, class Key,
          detail::GetNeighbors<Key> GetNeighbors,
          detail::GetDistance<Key> GetDistance, detail::IsTarget<Key> IsTarget>
std::pair<int, std::vector<Key>>
dijkstra(const Key &source, GetNeighbors &&get_neighbors,
         GetDistance &&get_distance, IsTarget &&is_target) {
    return dijkstra<use_visited>(source,
                                 std::forward<GetNeighbors>(get_neighbors),
                                 std::forward<GetDistance>(get_distance),
                                 std::forward<IsTarget>(is_target));
}

/**
 * Generic A* search on an arbitrary weighted graph.
 *
 * Returns the distance and path from the source to the first target found,
 * or -1 and an empty path if not found.
 */

namespace detail {
template <class Key>
struct a_star_entry {
    int estimate;
    int dist;
    Key key;

    a_star_entry(int estimate, int dist, const Key &key)
        : estimate(estimate), dist(dist), key(key) {}
    a_star_entry(int estimate, int dist, Key &&key)
        : estimate(estimate), dist(dist), key(std::move(key)) {}
    // a_star_entry(const a_star_entry &) = default;
    // a_star_entry(a_star_entry &&) = default;

    auto operator<=>(const a_star_entry &) const = default;
};
} // namespace detail

template <bool use_visited = false, class Key,
          detail::GetNeighbors<Key> GetNeighbors,
          detail::GetDistance<Key> GetDistance, detail::IsTarget<Key> IsTarget,
          detail::Heuristic<Key> Heuristic, detail::Visit<Key> Visit>
std::pair<int, std::vector<Key>>
a_star(const Key &source, GetNeighbors &&get_neighbors,
       GetDistance &&get_distance, IsTarget &&is_target, Heuristic &&heuristic,
       Visit &&visit) {
    using Entry = detail::a_star_entry<Key>;

    detail::maybe_unordered_set<Key> visited{};
    detail::maybe_unordered_map<Key, Entry> distances{};
    std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>>
        frontier{};

    distances.emplace(source, Entry{heuristic(source), 0, source});
    frontier.emplace(heuristic(source), 0, source);

    while (!frontier.empty()) {
        Entry curr = std::move(frontier.top());
        frontier.pop();
        {
            const Entry &existing = distances.at(curr.key);
            if (curr.dist != existing.dist) {
                continue;
            }
        }
        visit(curr.key, curr.dist);
        if (is_target(curr.key)) {
            // reconstruct path
            std::vector<Key> path{curr.key};
            typename decltype(distances)::const_iterator it;
            while (path.back() != source &&
                   (it = distances.find(path.back())) != distances.end()) {
                path.emplace_back(it->second.key);
            }
            std::ranges::reverse(path);
            return {curr.dist, path};
        }
        for (const Key &neighbor : get_neighbors(curr.key)) {
            if constexpr (use_visited) {
                if (visited.contains(neighbor)) {
                    continue;
                }
            }
            int new_distance = curr.dist + get_distance(curr.key, neighbor);
            auto it = distances.find(neighbor);
            if (it == distances.end() || new_distance < it->second.dist) {
                int new_estimate = new_distance + heuristic(neighbor);
                Entry new_entry{new_estimate, new_distance, curr.key};
                if (it != distances.end()) {
                    it->second = std::move(new_entry);
                } else {
                    distances.try_emplace(neighbor, std::move(new_entry));
                }
                frontier.emplace(new_estimate, new_distance, neighbor);
            }
        }
        if constexpr (use_visited) {
            visited.insert(std::move(curr.key));
        }
    }
    return {-1, {}};
}
template <bool use_visited = false, class Key,
          detail::GetNeighbors<Key> GetNeighbors,
          detail::GetDistance<Key> GetDistance, detail::IsTarget<Key> IsTarget,
          detail::Heuristic<Key> Heuristic>
std::pair<int, std::vector<Key>>
a_star(const Key &source, GetNeighbors &&get_neighbors,
       GetDistance &&get_distance, IsTarget &&is_target,
       Heuristic &&heuristic) {
    return a_star<use_visited>(
        source, std::forward<GetNeighbors>(get_neighbors),
        std::forward<GetDistance>(get_distance),
        std::forward<IsTarget>(is_target), std::forward<Heuristic>(heuristic),
        [](const Key &, int) {});
}

/**
 * Shortest distances from a single node using Dijkstra's algorithm.
 */
template <class Key, detail::GetNeighbors<Key> GetNeighbors,
          detail::GetDistance<Key> GetDistance>
detail::maybe_unordered_map<Key, int>
shortest_distances(const Key &source, GetNeighbors &&get_neighbors,
                   GetDistance &&get_distance) {
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

// instantiate templates in an anonymous namespace, so static analyzers will
// check these functions
namespace {
using Key1 = std::pair<int, int>;
[[maybe_unused]] void _lint_helper_unhashable(
    const Key1 &source,
    std::function<std::vector<Key1>(const Key1 &)> get_neighbors,
    std::function<bool(const Key1 &)> is_target,
    std::function<void(const Key1 &, int)> visit,
    std::function<void(const Key1 &, const Key1 &, int)> visit_with_parent,
    std::function<int(const Key1 &, const Key1 &)> get_distance,
    std::function<int(const Key1 &)> heuristic) {
    bfs<true>(source, get_neighbors, is_target);
    bfs<true>(source, get_neighbors, visit);
    bfs<true>(source, get_neighbors, is_target, visit);
    bfs<false>(source, get_neighbors, is_target);
    bfs<false>(source, get_neighbors, visit);
    bfs<false>(source, get_neighbors, is_target, visit);

    dfs<true>(source, get_neighbors, is_target);
    dfs<true>(source, get_neighbors, visit_with_parent);
    dfs<true>(source, get_neighbors, is_target, visit_with_parent);
    dfs<false>(source, get_neighbors, is_target);
    dfs<false>(source, get_neighbors, visit_with_parent);
    dfs<false>(source, get_neighbors, is_target, visit_with_parent);

    topo_sort(source, get_neighbors);

    longest_path_dag(source, get_neighbors, get_distance, is_target);

    dijkstra<false>(source, get_neighbors, get_distance, is_target);
    dijkstra<false>(source, get_neighbors, get_distance, is_target, visit);
    dijkstra<true>(source, get_neighbors, get_distance, is_target);
    dijkstra<true>(source, get_neighbors, get_distance, is_target, visit);

    a_star<false>(source, get_neighbors, get_distance, is_target, heuristic);
    a_star<false>(source, get_neighbors, get_distance, is_target, heuristic,
                  visit);
    a_star<true>(source, get_neighbors, get_distance, is_target, heuristic);
    a_star<true>(source, get_neighbors, get_distance, is_target, heuristic,
                 visit);

    shortest_distances(source, get_neighbors, get_distance);
}
using Key2 = int;
[[maybe_unused]] void _lint_helper_hashable(
    const Key2 &source,
    std::function<std::vector<Key2>(const Key2 &)> get_neighbors,
    std::function<bool(const Key2 &)> is_target,
    std::function<void(const Key2 &, int)> visit,
    std::function<void(const Key2 &, const Key2 &, int)> visit_with_parent,
    std::function<int(const Key2 &, const Key2 &)> get_distance,
    std::function<int(const Key2 &)> heuristic) {
    bfs<true>(source, get_neighbors, is_target);
    bfs<true>(source, get_neighbors, visit);
    bfs<true>(source, get_neighbors, is_target, visit);
    bfs<false>(source, get_neighbors, is_target);
    bfs<false>(source, get_neighbors, visit);
    bfs<false>(source, get_neighbors, is_target, visit);

    dfs<true>(source, get_neighbors, is_target);
    dfs<true>(source, get_neighbors, visit_with_parent);
    dfs<true>(source, get_neighbors, is_target, visit_with_parent);
    dfs<false>(source, get_neighbors, is_target);
    dfs<false>(source, get_neighbors, visit_with_parent);
    dfs<false>(source, get_neighbors, is_target, visit_with_parent);

    topo_sort(source, get_neighbors);

    longest_path_dag(source, get_neighbors, get_distance, is_target);

    dijkstra<false>(source, get_neighbors, get_distance, is_target);
    dijkstra<false>(source, get_neighbors, get_distance, is_target, visit);
    dijkstra<true>(source, get_neighbors, get_distance, is_target);
    dijkstra<true>(source, get_neighbors, get_distance, is_target, visit);

    a_star<false>(source, get_neighbors, get_distance, is_target, heuristic);
    a_star<false>(source, get_neighbors, get_distance, is_target, heuristic,
                  visit);
    a_star<true>(source, get_neighbors, get_distance, is_target, heuristic);
    a_star<true>(source, get_neighbors, get_distance, is_target, heuristic,
                 visit);

    shortest_distances(source, get_neighbors, get_distance);
}
} // namespace

} // namespace aoc::graph

#endif /* end of include guard: GRAPH_TRAVERSAL_HPP_56T9ZURK */
