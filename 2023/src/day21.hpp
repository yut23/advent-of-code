/******************************************************************************
 * File:        day21.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-21
 *****************************************************************************/

#ifndef DAY21_HPP_2X5T9V1Y
#define DAY21_HPP_2X5T9V1Y

#include "graph_traversal.hpp"
#include "lib.hpp"          // for AbsDirection, Pos, Delta, DIRECTIONS, DEBUG
#include <algorithm>        // for count_if
#include <array>            // for array
#include <cassert>          // for assert
#include <cstddef>          // for size_t
#include <initializer_list> // for initializer_list
#include <iostream>         // for istream, noskipws, cerr
#include <limits>           // for numeric_limits
#include <map>              // for map
#include <utility>          // for move, swap
#include <vector>           // for vector

namespace aoc::day21 {

template <class T>
struct Grid {
    using value_type = T;
    using size_type = int;

    const size_type height;
    const size_type width;
    std::vector<value_type> data;

    using reference = typename decltype(data)::reference;
    using const_reference = typename decltype(data)::const_reference;

    using iterator = typename decltype(data)::iterator;
    using const_iterator = typename decltype(data)::const_iterator;

    inline std::size_t get_index(size_type x, size_type y) const {
        return y * width + x;
    }

    constexpr Grid(size_type height, size_type width, const T &value = T())
        : height(height), width(width), data(height * width, value) {}
    explicit constexpr Grid(const std::vector<std::vector<T>> &grid)
        : height(grid.size()), width(grid.size() > 0 ? grid.front().size() : 0),
          data(height * width) {
        for (int y = 0; y < height; ++y) {
            const std::vector<T> &row = grid[y];
            assert(static_cast<size_type>(row.size()) == width);
            for (int x = 0; x < width; ++x) {
                data[get_index(x, y)] = row[x];
            }
        }
    }

    // constexpr Grid(const Grid &other) = default;
    // constexpr Grid(Grid &&other) noexcept = default;

    // constexpr Grid &operator=(const Grid &other) = default;
    // constexpr Grid &operator=(Grid &&other) noexcept = default;

    constexpr iterator begin() noexcept { return data.begin(); }
    constexpr const_iterator begin() const noexcept { return data.begin(); }
    constexpr const_iterator cbegin() const noexcept { return data.cbegin(); }

    constexpr iterator end() noexcept { return data.end(); }
    constexpr const_iterator end() const noexcept { return data.end(); }
    constexpr const_iterator cend() const noexcept { return data.cend(); }

    constexpr bool in_bounds(size_type x, size_type y) const noexcept {
        return y >= 0 && x >= 0 && y < height && x < width;
    }
    constexpr bool in_bounds(const Pos &pos) const noexcept {
        return in_bounds(pos.x, pos.y);
    }

    constexpr reference at(size_type x, size_type y) {
        return data.at(get_index(x, y));
    }
    constexpr reference at(const Pos &pos) { return at(pos.x, pos.y); }

    constexpr const_reference at(size_type x, size_type y) const {
        return data[get_index(x, y)];
    }
    constexpr const_reference at(const Pos &pos) const {
        return at(pos.x, pos.y);
    }

    constexpr reference operator[](const Pos &pos) {
        return data[get_index(pos.x, pos.y)];
    }
    constexpr const_reference operator[](const Pos &pos) const {
        return data[get_index(pos.x, pos.y)];
    }

    constexpr Pos wrap(const Pos &pos) const {
        return Pos((pos.x % width + width) % width,
                   (pos.y % height + height) % height);
    }
};

struct Garden {
    using Key = aoc::Pos;

    const Pos start;
    const Grid<bool> stones;

    Garden(const Pos &start, const std::vector<std::vector<bool>> &grid)
        : start(start), stones(grid) {
        assert(stones.in_bounds(start));
    }

    Grid<int> get_distances(const Key &source) const {
        Grid<int> distances(stones.height, stones.width,
                            std::numeric_limits<int>::max());
        distances[source] = 0;

        const auto get_neighbors = [this, &distances](const Key &key) {
            std::vector<Key> neighbors{};
            for (const AbsDirection &dir : aoc::DIRECTIONS) {
                Pos pos = key + Delta(dir, true);
                if (stones.in_bounds(pos) && stones[pos] &&
                    distances[pos] == std::numeric_limits<int>::max()) {
                    neighbors.push_back(pos);
                }
            }
            return neighbors;
        };
        const auto visit = [&distances](const Key &key, int distance) {
            distances[key] = distance;
        };

        aoc::graph::bfs(source, get_neighbors, visit);

        return distances;
    }

    bool check_part_2() const;

  public:
    int part_1() const;
    long part_2() const;
    static Garden read(std::istream &is);
};

int Garden::part_1() const {
    constexpr int target_distance = 64;

    const Grid<int> distances = get_distances(start);

    return std::count_if(distances.begin(), distances.end(), [](int distance) {
        return distance <= target_distance &&
               distance % 2 == target_distance % 2;
    });
}

bool Garden::check_part_2() const {
    // check that the grid is square
    bool square_grid = stones.width == stones.height;
    // check that the start position is in the center of the grid
    bool start_in_center =
        stones.width == start.x * 2 + 1 && stones.height == start.y * 2 + 1;

    // check that the distance from start to each corner is the same (it would
    // be fairly straightforward to handle if not)
    // We need a direct path from the start to each side
    bool direct_path_from_start = true;
    for (Pos pos = start; stones.in_bounds(pos); ++pos.x) {
        direct_path_from_start &= stones[pos];
    }
    for (Pos pos = start; stones.in_bounds(pos); --pos.x) {
        direct_path_from_start &= stones[pos];
    }
    for (Pos pos = start; stones.in_bounds(pos); ++pos.y) {
        direct_path_from_start &= stones[pos];
    }
    for (Pos pos = start; stones.in_bounds(pos); --pos.y) {
        direct_path_from_start &= stones[pos];
    }

    bool open_edges = true;
    for (int x = 0; x < stones.width; ++x) {
        open_edges &= stones.at(x, 0);
        open_edges &= stones.at(x, stones.height - 1);
    }
    for (int y = 0; y < stones.height; ++y) {
        open_edges &= stones.at(0, y);
        open_edges &= stones.at(stones.width - 1, y);
    }
    if constexpr (aoc::DEBUG) {
        std::cerr << "part 2 assumptions:\n";
        std::cerr << "  grid is square:                  "
                  << (square_grid ? "yes" : "no") << "\n";
        std::cerr << "  starting point at center:        "
                  << (start_in_center ? "yes" : "no") << "\n";
        std::cerr << "  direct path from start to edges: "
                  << (direct_path_from_start ? "yes" : "no") << "\n";
        std::cerr << "  edges are completely open:       "
                  << (open_edges ? "yes" : "no") << "\n";
    }

    return square_grid && start_in_center && direct_path_from_start &&
           open_edges;
}

class EdgeSet {
    struct Entry {
        Pos tile_pos;
        Pos start;
        int distance_left;
    };
    struct DistanceInfo {
        Grid<int> distances;
        int max_distance = 0;
        int reachable_odd = 0;
        int reachable_even = 0;

      private:
        std::array<std::pair<Pos, int>, 4> edge_min_dist;

      public:
        explicit DistanceInfo(const Grid<int> &distances)
            : distances(distances) {
            for (int distance : distances) {
                if (distance == std::numeric_limits<int>::max()) {
                    continue;
                }
                if (distance > max_distance) {
                    max_distance = distance;
                }
                if (distance % 2 == 0) {
                    ++reachable_even;
                } else {
                    ++reachable_odd;
                }
            }
            // find the minimum distance position for each edge
            for (AbsDirection dir : aoc::DIRECTIONS) {
                auto dir_idx =
                    static_cast<std::underlying_type_t<AbsDirection>>(dir);
                assert(dir_idx >= 0 && dir_idx < 4);
                Pos pos(0, 0);
                int min_dist = std::numeric_limits<int>::max();
                Pos min_pos;
                if (dir == AbsDirection::north || dir == AbsDirection::south) {
                    pos.y =
                        dir == AbsDirection::north ? 0 : distances.height - 1;
                    for (pos.x = 0; pos.x < distances.width; ++pos.x) {
                        int dist = distances[pos];
                        if (dist < min_dist) {
                            min_dist = dist;
                            min_pos = pos;
                        }
                    }
                } else {
                    pos.x = dir == AbsDirection::west ? 0 : distances.width - 1;
                    for (pos.y = 0; pos.y < distances.height; ++pos.y) {
                        int dist = distances[pos];
                        if (dist < min_dist) {
                            min_dist = dist;
                            min_pos = pos;
                        }
                    }
                }
                edge_min_dist[dir_idx] = std::make_pair(min_pos, min_dist);
            }
        }

        const std::pair<Pos, int> &get_edge_pos(AbsDirection dir) const {
            return edge_min_dist
                [static_cast<std::underlying_type_t<AbsDirection>>(dir)];
        }
    };

    const Garden &garden;
    std::map<Pos, Entry> edge_set;
    std::map<Pos, Entry> next_edge_set;

    std::map<Pos, const DistanceInfo> dist_info_cache;
    long reachable = 0;

  public:
    explicit EdgeSet(const Garden &garden_, int target_distance)
        : garden(garden_), edge_set(), next_edge_set() {
        add_tile(Pos(0, 0), garden.start, target_distance);
        std::swap(edge_set, next_edge_set);
    }

    const DistanceInfo &get_info(const Pos &start) {
        auto it_1 = dist_info_cache.find(start);
        if (it_1 != dist_info_cache.end()) {
            return it_1->second;
        }
        // add to cache
        auto it_2 =
            dist_info_cache
                .emplace(start, DistanceInfo(garden.get_distances(start)))
                .first;
        return it_2->second;
    }

  private:
    bool add_tile(const Pos &tile_pos, const Pos &start, int distance_left) {
        Entry new_entry{tile_pos, start, distance_left};
        auto it = next_edge_set.lower_bound(tile_pos);
        // replace existing entry if it has a lower remaining distance
        if (it != next_edge_set.end() && it->first == tile_pos) {
            if (it->second.distance_left < distance_left) {
                it->second = std::move(new_entry);
                return true;
            }
        } else {
            next_edge_set.emplace_hint(it, tile_pos, std::move(new_entry));
            return true;
        }
        return false;
    }

  public:
    void expand();

    bool empty() const { return edge_set.empty(); }
    std::size_t size() const { return edge_set.size(); }

    long get_reachable() const { return reachable; }

    friend std::ostream &operator<<(std::ostream &os, const Entry &);
};

std::ostream &operator<<(std::ostream &os, const EdgeSet::Entry &entry) {
    os << "Entry(tile=(" << entry.tile_pos.x << ", " << entry.tile_pos.y
       << "), start=(" << entry.start.x << ", " << entry.start.y
       << "), distance_left=" << entry.distance_left << ")";
    return os;
}

void EdgeSet::expand() {
    constexpr bool debug = aoc::DEBUG && false;
    for (const auto &[tile_pos, entry] : edge_set) {
        const DistanceInfo &info = get_info(entry.start);
        int prev_reachable = reachable;
        if (entry.distance_left >= info.max_distance) {
            // tile is fully reachable; use pre-calculated values
            if (entry.distance_left % 2 == 0) {
                reachable += info.reachable_even;
            } else {
                reachable += info.reachable_odd;
            }
        } else {
            // only part of the tile is reachable, so count the spaces manually
            const int target_distance = entry.distance_left;
            reachable +=
                std::count_if(info.distances.begin(), info.distances.end(),
                              [&target_distance](int distance) {
                                  return distance <= target_distance &&
                                         distance % 2 == target_distance % 2;
                              });
        }
        if constexpr (debug) {
            std::cerr << "processing " << entry << ": "
                      << (reachable - prev_reachable)
                      << " new reachable positions\n";
        } else {
            (void)prev_reachable;
        }
        const Grid<int> &distances = info.distances;
        for (const AbsDirection &dir : aoc::DIRECTIONS) {
            Delta delta(dir, true);
            Pos new_tile_pos = tile_pos + delta;
            if ((new_tile_pos - Pos(0, 0)).manhattan_distance() <=
                (tile_pos - Pos(0, 0)).manhattan_distance()) {
                // don't go backwards
                continue;
            }
            // add new tile, starting from the lowest distance value on the
            // adjacent edge
            const auto &[min_pos, min_dist] = info.get_edge_pos(dir);
            const int new_distance = entry.distance_left - (min_dist + 1);
            if (new_distance >= 0) {
                bool added =
                    add_tile(new_tile_pos, distances.wrap(min_pos + delta),
                             new_distance);
                if constexpr (debug) {
                    if (added) {
                        std::cerr << "  adding "
                                  << next_edge_set.at(new_tile_pos)
                                  << " to edge set from " << tile_pos << "\n";
                    }
                } else {
                    (void)added;
                }
            }
        }
    }
    edge_set.clear();
    std::swap(edge_set, next_edge_set);
    if constexpr (debug) {
        std::cerr << "edge set: ";
        for (auto it = edge_set.begin(); it != edge_set.end(); ++it) {
            if (it != edge_set.begin()) {
                std::cerr << ", ";
            }
            std::cerr << it->first;
        }
        std::cerr << "\n";
    }
}

long Garden::part_2() const {
    constexpr int target_distance = 26501365; // = 2023 * 100 * 131 + 65
    // constexpr int target_distance = 11;
    // constexpr int target_distance = 2023 * 1 * 131 + 65;

    // check our assumptions
    if (!check_part_2()) {
        return -1;
    }

    EdgeSet es(*this, target_distance);
    int i = 1;
    while (!es.empty()) {
        if constexpr (aoc::DEBUG) {
            std::cerr << "starting iteration " << i << " with " << es.size()
                      << " tiles in edge set...\n";
        }
        es.expand();
        ++i;
    }
    std::cerr << "took " << i << " iterations\n";
    return es.get_reachable();
}

Garden Garden::read(std::istream &is) {
    std::vector<std::vector<bool>> stones;
    Pos start;
    is >> std::noskipws;
    char ch;
    for (int y = 0; is; ++y) {
        for (int x = 0; is >> ch && ch != '\n'; ++x) {
            stones.resize(y + 1);
            stones[y].emplace_back(ch != '#');
            if (ch == 'S') {
                start = {x, y};
            }
        }
    }
    for (std::size_t y = 1; y < stones.size(); ++y) {
        assert(stones[y].size() == stones[y - 1].size());
    }
    return Garden(start, stones);
}

} // namespace aoc::day21

#endif /* end of include guard: DAY21_HPP_2X5T9V1Y */
