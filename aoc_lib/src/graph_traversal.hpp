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

#include "lib.hpp"           // for DEBUG
#include "util/concepts.hpp" // for Hashable, any_iterable_collection, same_as_any
#include "util/hash.hpp"     // for unordered_map_badness
#include <algorithm>         // for min, reverse
#include <cassert>           // for assert
#include <concepts>          // for same_as, integral
#include <functional>        // for function, greater
#include <initializer_list> // for initializer_list
#include <iostream>         // for cerr
#include <map>              // for map
#include <queue>            // for priority_queue
#include <set>              // for set
#include <stack>            // for stack
#include <stdexcept>        // for invalid_argument
#include <tuple>            // for tuple
#include <type_traits> // for conditional_t, invoke_result_t // IWYU pragma: export
#include <unordered_map> // for unordered_map
#include <unordered_set> // for unordered_set
#include <utility>       // for move, pair, make_pair, swap, forward
#include <vector>        // for vector
// IWYU pragma: no_include <compare>  // for operator==
// IWYU pragma: no_include <string>  // for operator==

// the export above avoids having to include type_traits in all source files
// for invoke_result_t

/// graph traversal algorithms
namespace aoc::graph {

namespace detail {

// placeholder type for optional handler functions
struct optional_func {
    // the compiler should optimize these out
    template <class Key>
    constexpr bool operator()(const Key &) {
        return false;
    };
    template <class Key>
    constexpr void operator()(const Key &, int){};
    template <class Key>
    constexpr void operator()(const Key &, const Key &, int){};
};

template <class Key>
using maybe_unordered_set =
    std::conditional_t<util::concepts::Hashable<Key>, std::unordered_set<Key>,
                       std::set<Key>>;

template <class Key, class T>
using maybe_unordered_map =
    std::conditional_t<util::concepts::Hashable<Key>,
                       std::unordered_map<Key, T>, std::map<Key, T>>;

template <class Func, class Key>
concept ProcessNeighbors =
    requires(Func process_neighbors, const Key &key,
             std::function<void(const Key &key)> &visitor) {
        process_neighbors(key, visitor);
    };

// cppcheck fails to parse this if the || comes after the requires statement
template <class Func, class Key>
concept IsTarget = std::same_as<Func, optional_func> ||
                   requires(Func is_target, const Key &key) {
                       { is_target(key) } -> std::same_as<bool>;
                   };

template <class Func, class Key>
concept Visit = std::same_as<Func, optional_func> ||
                requires(Func visit, const Key &key, int distance) {
                    {
                        visit(key, distance)
                    } -> util::concepts::same_as_any<void, bool>;
                };

template <class Func, class Key>
concept VisitWithParent = std::same_as<Func, optional_func> ||
                          requires(Func visit_with_parent, const Key &key,
                                   const Key &parent, int depth) {
                              {
                                  visit_with_parent(key, parent, depth)
                              } -> util::concepts::same_as_any<void, bool>;
                          };

template <class Key, class Func>
struct visit_invoke_result;

template <class Key>
struct visit_invoke_result<Key, optional_func> {
    using type = void;
};

template <class Key, Visit<Key> Func>
struct visit_invoke_result<Key, Func> {
    using type = std::invoke_result_t<Func &&, const Key &, int>;
};

template <class Key, VisitWithParent<Key> Func>
struct visit_invoke_result<Key, Func> {
    using type = std::invoke_result_t<Func &&, const Key &, const Key &, int>;
};

template <class Func, class Key>
concept GetDistance = requires(Func get_distance, const Key &u, const Key &v) {
    { get_distance(u, v) } -> std::integral;
};

template <class Func, class Key>
concept Heuristic = requires(Func heuristic, const Key &key) {
    { heuristic(key) } -> std::integral;
};

template <class T>
concept FuncPassed = !std::same_as<T, optional_func>;

template <class T>
concept AnySourceCollection =
    util::concepts::any_iterable_collection<T, typename T::value_type>;

} // namespace detail

/**
 * Generic BFS on an arbitrary graph.
 *
 * At least one of `is_target` and `visit` must be implemented.
 *
 * `visit(node, distance)` will be called for each node. It may optionally
 * return a bool, which if false, will stop processing that node (before it is
 * checked for being a target).
 *
 * If `use_seen` is true, nodes will only be visited once (i.e. a DAG will be
 * visited as a tree). `use_seen` should only be set to false if the graph has
 * no cycles.
 *
 * Returns the distance from the source(s) to the first target found, or -1 if
 * not found.
 */
template <bool use_seen = true, detail::AnySourceCollection ASC,
          class Key = typename ASC::value_type,
          detail::ProcessNeighbors<Key> ProcessNeighbors,
          detail::IsTarget<Key> IsTarget = detail::optional_func,
          detail::Visit<Key> Visit = detail::optional_func>
int bfs(const ASC &sources, ProcessNeighbors &&process_neighbors,
        IsTarget &&is_target, Visit &&visit) {
    static_assert(detail::FuncPassed<IsTarget> || detail::FuncPassed<Visit>,
                  "is_target and visit must not both be defaulted");
    using visit_ret_t = typename detail::visit_invoke_result<Key, Visit>::type;
    detail::maybe_unordered_set<Key> queue = {std::begin(sources),
                                              std::end(sources)};
    detail::maybe_unordered_set<Key> next_queue{};
    detail::maybe_unordered_set<Key> seen{};

    for (int distance = 0; !queue.empty();
         ++distance, queue.clear(), std::swap(queue, next_queue)) {
        for (const Key &key : queue) {
            if constexpr (std::same_as<visit_ret_t, bool>) {
                if (!visit(key, distance)) {
                    continue;
                }
            } else {
                visit(key, distance);
            }
            if (is_target(key)) {
                return distance;
            }
            if constexpr (use_seen) {
                seen.insert(key);
            }
            process_neighbors(key, [&seen, &next_queue](const Key &neighbor) {
                if constexpr (use_seen) {
                    if (seen.contains(neighbor)) {
                        return;
                    }
                } else {
                    // suppress unused lambda capture warning
                    (void)seen;
                }
                next_queue.insert(neighbor);
            });
        }
    }
    return -1;
}

// specialization for std::initializer_list, so deduction of `bfs({source,
// source}, ...)` works
template <bool use_seen = true, class Key,
          detail::ProcessNeighbors<Key> ProcessNeighbors,
          detail::IsTarget<Key> IsTarget = detail::optional_func,
          detail::Visit<Key> Visit = detail::optional_func>
int bfs(const std::initializer_list<Key> &sources,
        ProcessNeighbors &&process_neighbors, IsTarget &&is_target,
        Visit &&visit) {
    // explicitly specify the ASC template argument to avoid recursion
    return bfs<use_seen, std::initializer_list<Key>>(
        sources, std::forward<ProcessNeighbors>(process_neighbors),
        std::forward<IsTarget>(is_target), std::forward<Visit>(visit));
}

template <bool use_seen = true, class Key,
          detail::ProcessNeighbors<Key> ProcessNeighbors,
          detail::IsTarget<Key> IsTarget = detail::optional_func,
          detail::Visit<Key> Visit = detail::optional_func>
int bfs(const Key &source, ProcessNeighbors &&process_neighbors,
        IsTarget &&is_target, Visit &&visit) {
    return bfs<use_seen, std::initializer_list<Key>>(
        {source}, std::forward<ProcessNeighbors>(process_neighbors),
        std::forward<IsTarget>(is_target), std::forward<Visit>(visit));
}

/**
 * Generic BFS on an arbitrary graph, with no duplicate checking.
 *
 * Functionally equivalent to bfs<false>(...), but uses more efficient data
 * structures to store the pending nodes.
 *
 * At least one of `is_target` and `visit` must be implemented.
 *
 * `visit(node, distance)` will be called for each node. It may optionally
 * return a bool, which if false, will stop processing that node (before it is
 * checked for being a target).
 *
 * Returns the distance from the source to the first target found, or -1 if not
 * found.
 */
template <class Key, detail::ProcessNeighbors<Key> ProcessNeighbors,
          detail::IsTarget<Key> IsTarget = detail::optional_func,
          detail::Visit<Key> Visit = detail::optional_func>
int bfs_manual_dedupe(const Key &source, ProcessNeighbors &&process_neighbors,
                      IsTarget &&is_target, Visit &&visit) {
    static_assert(detail::FuncPassed<IsTarget> || detail::FuncPassed<Visit>,
                  "is_target and visit must not both be defaulted");
    using visit_ret_t = typename detail::visit_invoke_result<Key, Visit>::type;
    std::vector<Key> queue = {source};
    std::vector<Key> next_queue{};

    for (int distance = 0; !queue.empty();
         ++distance, queue.clear(), std::swap(queue, next_queue)) {
        for (const Key &key : queue) {
            if constexpr (std::same_as<visit_ret_t, bool>) {
                if (!visit(key, distance)) {
                    continue;
                }
            } else {
                visit(key, distance);
            }
            if (is_target(key)) {
                return distance;
            }
            process_neighbors(key, [&next_queue](const Key &neighbor) {
                next_queue.push_back(neighbor);
            });
        }
    }
    return -1;
}

/**
 * Generic DFS on an arbitrary graph, non-recursive version.
 *
 * At least one of `is_target` and `visit_with_parent` must be implemented.
 *
 * `visit_with_parent(node, parent, depth)` will be called for each node. It
 * may optionally return a bool, which if false, will stop processing that node
 * (before it is checked for being a target).
 *
 * If `use_seen` is true, nodes will only be visited once (i.e. a DAG will be
 * visited as a tree). `use_seen` should only be set to false if the graph has
 * no cycles.
 *
 * Returns the distance from the source to the first target found, or -1 if not
 * found.
 */
template <bool use_seen = true, class Key,
          detail::ProcessNeighbors<Key> ProcessNeighbors,
          detail::IsTarget<Key> IsTarget = detail::optional_func,
          detail::VisitWithParent<Key> VisitWithParent = detail::optional_func>
int dfs(const Key &source, ProcessNeighbors &&process_neighbors,
        IsTarget &&is_target, VisitWithParent &&visit_with_parent) {
    static_assert(detail::FuncPassed<IsTarget> ||
                      detail::FuncPassed<VisitWithParent>,
                  "is_target and visit_with_parent must not both be defaulted");
    using visit_ret_t =
        typename detail::visit_invoke_result<Key, VisitWithParent>::type;
    std::stack<std::tuple<Key, Key, int>> stack{};
    stack.emplace(source, source, 0);
    detail::maybe_unordered_set<Key> seen{};

    while (!stack.empty()) {
        const auto [key, parent, depth] = std::move(stack.top());
        stack.pop();
        if constexpr (std::same_as<visit_ret_t, bool>) {
            if (!visit_with_parent(key, parent, depth)) {
                continue;
            }
        } else {
            visit_with_parent(key, parent, depth);
        }
        if (is_target(key)) {
            return stack.size() - 1;
        }
        if constexpr (use_seen) {
            seen.insert(key);
        }
        process_neighbors(key, [&seen, &stack, &key = key,
                                depth = depth](const Key &neighbor) {
            if constexpr (use_seen) {
                if (seen.contains(neighbor)) {
                    return;
                }
            } else {
                // suppress unused lambda capture warning
                (void)seen;
            }
            stack.emplace(neighbor, key, depth + 1);
        });
    }
    return -1;
}

/**
 * Generic DFS on an arbitrary graph, recursive version.
 *
 * Uses less memory than the non-recursive version, but requires
 * `process_neighbors` to be reentrant.
 *
 * At least one of `is_target` and `visit_with_parent` must be implemented.
 *
 * `visit_with_parent(node, parent, depth)` will be called for each node. It
 * may optionally return a bool, which if false, will stop processing that node
 * (before it is checked for being a target).
 *
 * If `use_seen` is true, nodes will only be visited once (i.e. a DAG will be
 * visited as a tree). `use_seen` should only be set to false if the graph has
 * no cycles.
 *
 * Returns the distance from the source to the first target found, or -1 if not
 * found.
 */
template <bool use_seen = true, class Key,
          detail::ProcessNeighbors<Key> ProcessNeighbors,
          detail::IsTarget<Key> IsTarget = detail::optional_func,
          detail::VisitWithParent<Key> VisitWithParent = detail::optional_func>
int dfs_rec(const Key &source, ProcessNeighbors &&process_neighbors,
            IsTarget &&is_target, VisitWithParent &&visit_with_parent) {
    static_assert(detail::FuncPassed<IsTarget> ||
                      detail::FuncPassed<VisitWithParent>,
                  "is_target and visit_with_parent must not both be defaulted");
    using visit_ret_t =
        typename detail::visit_invoke_result<Key, VisitWithParent>::type;
    detail::maybe_unordered_set<Key> seen{};

    auto helper = [&seen, &process_neighbors, &is_target,
                   &visit_with_parent](const Key &key, int depth,
                                       const Key &parent, const auto &rec) {
        if constexpr (std::same_as<visit_ret_t, bool>) {
            if (!visit_with_parent(key, parent, depth)) {
                return -1;
            }
        } else {
            visit_with_parent(key, parent, depth);
        }
        if (is_target(key)) {
            return depth;
        }
        if constexpr (use_seen) {
            seen.insert(key);
        } else {
            // suppress unused lambda capture warning
            (void)seen;
        }
        int result = -1;
        process_neighbors(
            key, [&rec, &seen, &result, &key, depth](const Key &neighbor) {
                if (result != -1) {
                    // found target in a previous iteration
                    return;
                }
                if constexpr (use_seen) {
                    if (seen.contains(neighbor)) {
                        return;
                    }
                } else {
                    // suppress unused lambda capture warning
                    (void)seen;
                }
                result = rec(neighbor, depth + 1, key, rec);
            });
        return result;
    };

    return helper(source, 0, source, helper);
}

/**
 * Topologically sort a directed acyclic graph.
 *
 * Throws a std::invalid_argument exception if the graph contains any cycles.
 */
template <class Key, detail::ProcessNeighbors<Key> ProcessNeighbors>
std::vector<Key> topo_sort(const Key &source,
                           ProcessNeighbors &&process_neighbors) {
    detail::maybe_unordered_set<Key> temp_marks{};
    detail::maybe_unordered_set<Key> perm_marks{};

    std::vector<Key> ordered{};

    auto visit = [&process_neighbors, &temp_marks, &perm_marks,
                  &ordered](auto &rec, const Key &n) -> void {
        if (perm_marks.contains(n)) {
            return;
        }
        if (temp_marks.contains(n)) {
            throw std::invalid_argument("graph contains at least one cycle");
        }
        temp_marks.emplace(n);
        process_neighbors(n, std::bind_front(rec, rec));
        perm_marks.insert(temp_marks.extract(n));
        ordered.emplace_back(n);
    };

    visit(visit, source);

    std::ranges::reverse(ordered);

    return ordered;
}

namespace detail {
struct tarjan_entry {
    int index = -1;
    int lowlink = -1;
    int component_id = -1;
    std::vector<int> pending_edges;
};
} // namespace detail

/**
 * Find strongly connected components of a directed graph.
 *
 * Components are returned in topological order, along with a set of the
 * directed edges between the components.
 */
template <detail::AnySourceCollection ASC, class Key = typename ASC::value_type,
          detail::ProcessNeighbors<Key> ProcessNeighbors>
std::pair<std::vector<std::vector<Key>>, std::set<std::pair<int, int>>>
tarjan_scc(const ASC &sources, ProcessNeighbors &&process_neighbors) {
    int index = 0;
    std::stack<Key> S{};
    std::vector<std::vector<Key>> components{};

    detail::maybe_unordered_map<Key, detail::tarjan_entry> entries;
    std::set<std::pair<int, int>> component_links;

    const auto strongconnect =
        [&process_neighbors, &index, &S, &components, &entries,
         &component_links](const Key &v, auto &rec) -> detail::tarjan_entry & {
        detail::tarjan_entry &v_entry = entries[v];
        v_entry.index = index;
        v_entry.lowlink = index;
        ++index;
        S.push(v);
        assert(v_entry.component_id == -1);

        process_neighbors(v, [&entries, &rec, &v_entry](const Key &w) {
            detail::tarjan_entry *w_entry = nullptr;
            auto it = entries.find(w);
            if (it == entries.end()) {
                // Successor w has not yet been visited; recurse on it
                w_entry = &rec(w, rec);
                v_entry.lowlink = std::min(v_entry.lowlink, w_entry->lowlink);
            } else {
                w_entry = &it->second;
                if (it->second.component_id == -1) {
                    // Successor w is in stack S and hence in the current SCC
                    // If w is not on stack, then (v, w) is an edge pointing to
                    // an SCC already found and must be ignored

                    v_entry.lowlink = std::min(v_entry.lowlink, w_entry->index);
                }
            }
            if (w_entry->component_id != -1) {
                v_entry.pending_edges.push_back(w_entry->component_id);
            }
        });
        // If v is a root node, pop the stack and generate an SCC
        if (v_entry.lowlink == v_entry.index) {
            int component_id = components.size();
            components.emplace_back();
            Key w = S.top();
            do {
                w = S.top();
                S.pop();
                auto &entry = entries.at(w);
                entry.component_id = component_id;
                for (int x : entry.pending_edges) {
                    component_links.emplace(component_id, x);
                }
                components.back().push_back(w);
            } while (w != v);
        }
        return v_entry;
    };

    for (const auto &source : sources) {
        if (!entries.contains(source)) {
            strongconnect(source, strongconnect);
        }
    }

    // check edges
    if constexpr (aoc::DEBUG) {
        // reconstruct the links between components manually
        std::set<std::pair<int, int>> reconstructed_links;
        for (const auto &[v, v_entry] : entries) {
            process_neighbors(v, [&entries, &reconstructed_links,
                                  &v_entry = v_entry](const Key &w) {
                const auto &w_entry = entries.at(w);
                if (v_entry.component_id != w_entry.component_id) {
                    reconstructed_links.emplace(v_entry.component_id,
                                                w_entry.component_id);
                }
            });
        }
        assert(component_links == reconstructed_links);
    }

    std::ranges::reverse(components);
    std::set<std::pair<int, int>> reversed_links;
    for (const auto &[v_id, w_id] : component_links) {
        reversed_links.emplace(components.size() - 1 - v_id,
                               components.size() - 1 - w_id);
    }
    return {std::move(components), std::move(reversed_links)};
}

template <class Key, detail::ProcessNeighbors<Key> ProcessNeighbors>
std::pair<std::vector<std::vector<Key>>, std::set<std::pair<int, int>>>
tarjan_scc(const std::initializer_list<Key> &sources,
           ProcessNeighbors &&process_neighbors) {
    return tarjan_scc<std::initializer_list<Key>>(
        sources, std::forward<ProcessNeighbors>(process_neighbors));
}

template <class Key, detail::ProcessNeighbors<Key> ProcessNeighbors>
std::pair<std::vector<std::vector<Key>>, std::set<std::pair<int, int>>>
tarjan_scc(const Key &source, ProcessNeighbors &&process_neighbors) {
    const std::initializer_list<Key> sources = {source};
    return tarjan_scc(sources,
                      std::forward<ProcessNeighbors>(process_neighbors));
}

/**
 * Longest path algorithm for a DAG.
 */
template <class Key, detail::ProcessNeighbors<Key> ProcessNeighbors,
          detail::GetDistance<Key> GetDistance, detail::IsTarget<Key> IsTarget>
std::pair<int, std::vector<Key>>
longest_path_dag(const Key &source, ProcessNeighbors &&process_neighbors,
                 GetDistance &&get_distance, IsTarget &&is_target) {
    // find all the incoming neighbors of each node
    detail::maybe_unordered_map<Key, detail::maybe_unordered_set<Key>>
        incoming_neighbors{};
    incoming_neighbors[source] = {};
    constexpr bool use_seen = false;
    dfs<use_seen>(
        source, process_neighbors, /*is_target*/ {},
        [&incoming_neighbors](const Key &node, const Key &parent, int) {
            if (node != parent) {
                incoming_neighbors[node].emplace(parent);
            }
        });

    std::vector<Key> ordering = topo_sort<Key>(source, process_neighbors);
    detail::maybe_unordered_map<Key, std::pair<int, Key>> longest_path{};

    // find longest path from source to each node
    longest_path.emplace(source, std::make_pair(0, source));
    for (const auto &key : ordering) {
        std::pair<int, Key> max_distance = {0, key};
        for (const auto &parent : incoming_neighbors.at(key)) {
            int new_distance = get_distance(parent, key);
            const auto it = longest_path.find(parent);
            if (it != longest_path.end()) {
                new_distance += it->second.first;
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
    return {max_distance, std::move(path)};
}

/**
 * Generic Dijkstra's algorithm on an arbitrary weighted graph.
 *
 * Returns the distance and path from the source to the first target found,
 * or -1 and an empty path if not found.
 */
template <bool use_visited = false, class Key,
          detail::ProcessNeighbors<Key> ProcessNeighbors,
          detail::GetDistance<Key> GetDistance, detail::IsTarget<Key> IsTarget,
          detail::Visit<Key> Visit = detail::optional_func>
std::pair<int, std::vector<Key>>
dijkstra(const Key &source, ProcessNeighbors &&process_neighbors,
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
            if constexpr (aoc::DEBUG && util::concepts::Hashable<Key>) {
                std::cerr << "distances map badness: "
                          << util::hash::unordered_map_badness(distances)
                          << "\n";
            }
            return {dist, path};
        }
        process_neighbors(current, [&get_distance, &visited, &distances,
                                    &frontier, dist = dist,
                                    &current = current](const Key &neighbor) {
            if constexpr (use_visited) {
                if (visited.contains(neighbor)) {
                    return;
                }
            } else {
                // suppress unused lambda capture warning
                (void)visited;
            }
            int new_distance = dist + get_distance(current, neighbor);
            auto it = distances.find(neighbor);
            if (it == distances.end() || new_distance < it->second.first) {
                std::pair<int, Key> value{new_distance, current};
                if (it != distances.end()) {
                    // this path has a shorter distance than one we found before
                    it->second = std::move(value);
                } else {
                    // we've never seen this node before
                    distances.try_emplace(neighbor, std::move(value));
                }
                frontier.emplace(new_distance, neighbor);
            }
        });
        if constexpr (use_visited) {
            visited.insert(std::move(current));
        }
    }
    return {-1, {}};
}

namespace detail {
template <class Key>
struct a_star_entry {
    int estimate;
    int dist;
    Key key;
    bool visited = false;

    a_star_entry(int estimate, int dist, const Key &key)
        : estimate(estimate), dist(dist), key(key) {}

    auto operator<=>(const a_star_entry &) const = default;
};
} // namespace detail

/**
 * Generic A* search on an arbitrary weighted graph.
 *
 * Returns the distance and path from the source to the first target found,
 * or -1 and an empty path if not found.
 */
template <bool use_visited = false, class Key,
          detail::ProcessNeighbors<Key> ProcessNeighbors,
          detail::GetDistance<Key> GetDistance, detail::IsTarget<Key> IsTarget,
          detail::Heuristic<Key> Heuristic,
          detail::Visit<Key> Visit = detail::optional_func>
std::pair<int, std::vector<Key>>
a_star(const Key &source, ProcessNeighbors &&process_neighbors,
       GetDistance &&get_distance, IsTarget &&is_target, Heuristic &&heuristic,
       Visit &&visit) {
    using Entry = detail::a_star_entry<Key>;

    detail::maybe_unordered_map<Key, Entry> distances{};
    std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>>
        frontier{};

    distances.emplace(source, Entry{heuristic(source), 0, source});
    frontier.emplace(heuristic(source), 0, source);

    while (!frontier.empty()) {
        Entry curr = std::move(frontier.top());
        frontier.pop();
        Entry &distances_entry = distances.at(curr.key);
        if (curr.dist != distances_entry.dist) {
            continue;
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
            return {curr.dist, std::move(path)};
        }
        process_neighbors(curr.key, [&get_distance, &heuristic, &distances,
                                     &frontier, &curr](const Key &neighbor) {
            auto it = distances.find(neighbor);
            if constexpr (use_visited) {
                if (it != distances.end() && it->second.visited) {
                    return;
                }
            }
            int new_distance = curr.dist + get_distance(curr.key, neighbor);
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
        });
        if constexpr (use_visited) {
            distances_entry.visited = true;
        }
    }
    return {-1, {}};
}

/**
 * Shortest distances from a single node using Dijkstra's algorithm.
 */
template <class Key, detail::ProcessNeighbors<Key> ProcessNeighbors,
          detail::GetDistance<Key> GetDistance>
detail::maybe_unordered_map<Key, int>
shortest_distances(const Key &source, ProcessNeighbors &&process_neighbors,
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
        process_neighbors(current, [&get_distance, &visited, &distances, &pq,
                                    dist = dist,
                                    &current = current](const Key &neighbor) {
            if (visited.contains(neighbor)) {
                return;
            }
            int new_distance = dist + get_distance(current, neighbor);
            auto it = distances.find(neighbor);
            if (it == distances.end() || it->second > new_distance) {
                distances[neighbor] = new_distance;
                pq.emplace(new_distance, neighbor);
            }
        });
        visited.insert(std::move(current));
    }
    return distances;
}

// instantiate templates in an anonymous namespace, so static analyzers will
// check these functions
namespace {
template <class Key>
void _lint_helper_template(
    const Key &source,
    std::function<void(const Key &, std::function<void(const Key &)>)>
        process_neighbors,
    std::function<bool(const Key &)> is_target,
    std::function<void(const Key &, int)> visit,
    std::function<bool(const Key &, int)> visit_bool,
    std::function<void(const Key &, const Key &, int)> visit_with_parent,
    std::function<bool(const Key &, const Key &, int)> visit_with_parent_bool,
    std::function<int(const Key &, const Key &)> get_distance,
    std::function<int(const Key &)> heuristic) {
    const std::vector<Key> sources_vec{source, source};

    bfs<true>(source, process_neighbors, is_target, {});
    bfs<true>(source, process_neighbors, {}, visit);
    bfs<true>(source, process_neighbors, {}, visit_bool);
    bfs<true>(source, process_neighbors, is_target, visit);
    bfs<true>(source, process_neighbors, is_target, visit_bool);
    bfs<false>(source, process_neighbors, is_target, {});
    bfs<false>(source, process_neighbors, {}, visit);
    bfs<false>(source, process_neighbors, {}, visit_bool);
    bfs<false>(source, process_neighbors, is_target, visit);
    bfs<false>(source, process_neighbors, is_target, visit_bool);

    bfs<true>({source, source}, process_neighbors, is_target, {});
    bfs<true>({source, source}, process_neighbors, {}, visit);
    bfs<true>({source, source}, process_neighbors, {}, visit_bool);
    bfs<true>({source, source}, process_neighbors, is_target, visit);
    bfs<true>({source, source}, process_neighbors, is_target, visit_bool);
    bfs<false>({source, source}, process_neighbors, is_target, {});
    bfs<false>({source, source}, process_neighbors, {}, visit);
    bfs<false>({source, source}, process_neighbors, {}, visit_bool);
    bfs<false>({source, source}, process_neighbors, is_target, visit);
    bfs<false>({source, source}, process_neighbors, is_target, visit_bool);

    bfs<true>(sources_vec, process_neighbors, is_target, {});
    bfs<true>(sources_vec, process_neighbors, {}, visit);
    bfs<true>(sources_vec, process_neighbors, {}, visit_bool);
    bfs<true>(sources_vec, process_neighbors, is_target, visit);
    bfs<true>(sources_vec, process_neighbors, is_target, visit_bool);
    bfs<false>(sources_vec, process_neighbors, is_target, {});
    bfs<false>(sources_vec, process_neighbors, {}, visit);
    bfs<false>(sources_vec, process_neighbors, {}, visit_bool);
    bfs<false>(sources_vec, process_neighbors, is_target, visit);
    bfs<false>(sources_vec, process_neighbors, is_target, visit_bool);

    bfs_manual_dedupe(source, process_neighbors, is_target, {});
    bfs_manual_dedupe(source, process_neighbors, {}, visit);
    bfs_manual_dedupe(source, process_neighbors, {}, visit_bool);
    bfs_manual_dedupe(source, process_neighbors, is_target, visit);
    bfs_manual_dedupe(source, process_neighbors, is_target, visit_bool);

    dfs<true>(source, process_neighbors, is_target, {});
    dfs<true>(source, process_neighbors, {}, visit_with_parent);
    dfs<true>(source, process_neighbors, {}, visit_with_parent_bool);
    dfs<true>(source, process_neighbors, is_target, visit_with_parent);
    dfs<true>(source, process_neighbors, is_target, visit_with_parent_bool);
    dfs<false>(source, process_neighbors, is_target, {});
    dfs<false>(source, process_neighbors, {}, visit_with_parent);
    dfs<false>(source, process_neighbors, {}, visit_with_parent_bool);
    dfs<false>(source, process_neighbors, is_target, visit_with_parent);
    dfs<false>(source, process_neighbors, is_target, visit_with_parent_bool);

    dfs_rec<true>(source, process_neighbors, is_target, {});
    dfs_rec<true>(source, process_neighbors, {}, visit_with_parent);
    dfs_rec<true>(source, process_neighbors, {}, visit_with_parent_bool);
    dfs_rec<true>(source, process_neighbors, is_target, visit_with_parent);
    dfs_rec<true>(source, process_neighbors, is_target, visit_with_parent_bool);
    dfs_rec<false>(source, process_neighbors, is_target, {});
    dfs_rec<false>(source, process_neighbors, {}, visit_with_parent);
    dfs_rec<false>(source, process_neighbors, {}, visit_with_parent_bool);
    dfs_rec<false>(source, process_neighbors, is_target, visit_with_parent);
    dfs_rec<false>(source, process_neighbors, is_target,
                   visit_with_parent_bool);

    topo_sort(source, process_neighbors);

    tarjan_scc(source, process_neighbors);
    tarjan_scc({source, source}, process_neighbors);
    tarjan_scc(sources_vec, process_neighbors);

    longest_path_dag(source, process_neighbors, get_distance, is_target);

    dijkstra<false>(source, process_neighbors, get_distance, is_target, {});
    dijkstra<false>(source, process_neighbors, get_distance, is_target, visit);
    dijkstra<true>(source, process_neighbors, get_distance, is_target, {});
    dijkstra<true>(source, process_neighbors, get_distance, is_target, visit);

    a_star<false>(source, process_neighbors, get_distance, is_target, heuristic,
                  {});
    a_star<false>(source, process_neighbors, get_distance, is_target, heuristic,
                  visit);
    a_star<true>(source, process_neighbors, get_distance, is_target, heuristic,
                 {});
    a_star<true>(source, process_neighbors, get_distance, is_target, heuristic,
                 visit);

    shortest_distances(source, process_neighbors, get_distance);
}
using Key1 = std::pair<int, int>;
[[maybe_unused]] void _lint_helper_unhashable(
    const Key1 &source,
    std::function<void(const Key1 &, std::function<void(const Key1 &)>)>
        process_neighbors,
    std::function<bool(const Key1 &)> is_target,
    std::function<void(const Key1 &, int)> visit,
    std::function<bool(const Key1 &, int)> visit_bool,
    std::function<void(const Key1 &, const Key1 &, int)> visit_with_parent,
    std::function<bool(const Key1 &, const Key1 &, int)> visit_with_parent_bool,
    std::function<int(const Key1 &, const Key1 &)> get_distance,
    std::function<int(const Key1 &)> heuristic) {
    _lint_helper_template(source, process_neighbors, is_target, visit,
                          visit_bool, visit_with_parent, visit_with_parent_bool,
                          get_distance, heuristic);
}
using Key2 = int;
[[maybe_unused]] void _lint_helper_hashable(
    const Key2 &source,
    std::function<void(const Key2 &, std::function<void(const Key2 &)>)>
        process_neighbors,
    std::function<bool(const Key2 &)> is_target,
    std::function<void(const Key2 &, int)> visit,
    std::function<bool(const Key2 &, int)> visit_bool,
    std::function<void(const Key2 &, const Key2 &, int)> visit_with_parent,
    std::function<bool(const Key2 &, const Key2 &, int)> visit_with_parent_bool,
    std::function<int(const Key2 &, const Key2 &)> get_distance,
    std::function<int(const Key2 &)> heuristic) {
    _lint_helper_template(source, process_neighbors, is_target, visit,
                          visit_bool, visit_with_parent, visit_with_parent_bool,
                          get_distance, heuristic);
}
} // namespace

} // namespace aoc::graph

#endif /* end of include guard: GRAPH_TRAVERSAL_HPP_56T9ZURK */
