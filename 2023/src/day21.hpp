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
#include <cstdlib>          // for abs, size_t
#include <initializer_list> // for initializer_list
#include <iostream>         // for istream, noskipws, cerr
#include <limits>           // for numeric_limits
#include <map>              // for map
#include <utility>          // for pair, move, swap
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

struct EdgeSet {
    struct Entry {
        Pos tile_pos;
        Pos start;
        int distance_left;
    };
    struct DistanceInfo {
        Grid<int> distances;
        int max_distance = 0;
        std::array<int, 2> total_reachable{0, 0};

      private:
        std::array<std::pair<Pos, int>, 4> edge_min_dist;

      public:
        explicit DistanceInfo(const Grid<int> &distances_)
            : distances(distances_) {
            for (int distance : distances) {
                if (distance == std::numeric_limits<int>::max()) {
                    continue;
                }
                if (distance > max_distance) {
                    max_distance = distance;
                }
                ++total_reachable[distance % 2];
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
    const int target_distance;
    std::vector<Entry> edge_set;
    std::vector<Entry> next_edge_set;

    std::map<Pos, const DistanceInfo> dist_info_cache;
    std::map<std::pair<Pos, int>, int> reachable_cache;
    std::size_t reachable_cache_accesses = 0;
    long reachable = 0;
    long tiles_visited = 0;

  public:
    explicit EdgeSet(const Garden &garden_, int target_distance_)
        : garden(garden_), target_distance(target_distance_), edge_set(),
          next_edge_set() {
        // add_tile(Pos(0, 0), garden.start, target_distance);
        add_tile(Pos(0, 0));
        std::swap(edge_set, next_edge_set);
    }

    // void add_tile(const Pos &tile_pos, const Pos &start, int distance_left);
    void add_tile(const Pos &tile_pos);

    // private:
    const DistanceInfo &get_info(const Pos &start) {
        auto it = dist_info_cache.lower_bound(start);
        if (it == dist_info_cache.end() || it->first != start) {
            // add to cache
            it = dist_info_cache.emplace_hint(
                it, start, DistanceInfo(garden.get_distances(start)));
        }
        return it->second;
    }

  public:
    void expand();
    void skip_to_edge();

    bool empty() const { return edge_set.empty(); }
    std::size_t size() const { return edge_set.size(); }

    long get_reachable() const { return reachable; }

    void print_stats() const {
        std::cerr << "distance info cache size: " << dist_info_cache.size()
                  << "\n";
        std::cerr << "reachable cache size: " << reachable_cache.size()
                  << "; accesses: " << reachable_cache_accesses << "\n";
        std::cerr << "visited " << tiles_visited << " tiles\n";
    }
};

std::ostream &operator<<(std::ostream &os, const EdgeSet::Entry &entry) {
    os << "Entry(tile=(" << entry.tile_pos.x << ", " << entry.tile_pos.y
       << "), start=(" << entry.start.x << ", " << entry.start.y
       << "), distance_left=" << entry.distance_left << ")";
    return os;
}

// void EdgeSet::add_tile(const Pos &tile_pos, const Pos &start_,
//                        int distance_left_) {
void EdgeSet::add_tile(const Pos &tile_pos) {
    // TODO: try calculating start and distance_left from tile_pos, to make sure
    // we are doing it correctly
    Pos start = garden.start;
    int distance_left = target_distance;
    const int width = garden.stones.width;
    const int height = garden.stones.height;

    if (tile_pos.x > 0) {
        start.x = 0;
        distance_left -= width - garden.start.x;
    } else if (tile_pos.x < 0) {
        start.x = width - 1;
        distance_left -= garden.start.x + 1;
    }
    distance_left -= std::max(0, std::abs(tile_pos.x) - 1) * width;

    if (tile_pos.y > 0) {
        start.y = 0;
        distance_left -= height - garden.start.y;
    } else if (tile_pos.y < 0) {
        start.y = height - 1;
        distance_left -= garden.start.y + 1;
    }
    distance_left -= std::max(0, std::abs(tile_pos.y) - 1) * height;

    // assert(start == start_);
    // assert(distance_left == distance_left_);
    // const int tile_dist = (tile_pos - Pos(0, 0)).manhattan_distance();
    // const int actual_parity = distance_left % 2;
    // const int manual_parity =
    //    (tile_dist % 2) ^ static_cast<int>(tile_pos.x == 0) ^
    //    static_cast<int>(tile_pos.y == 0) ^ (target_distance % 2);
    // if (actual_parity != manual_parity) {
    //    std::cerr << "tile_pos = " << tile_pos
    //              << ", distance_left = " << distance_left << "\n"
    //              << "actual_parity = " << actual_parity
    //              << ", manual_parity = " << manual_parity << "\n";
    //    assert(false);
    // }
    if (distance_left >= 0) {
        next_edge_set.push_back(
            Entry{tile_pos, std::move(start), distance_left});
    }
}

void EdgeSet::expand() {
    constexpr bool debug = aoc::DEBUG && false;
    std::vector<AbsDirection> dirs;
    for (const auto &entry : edge_set) {
        const Pos &tile_pos = entry.tile_pos;
        const DistanceInfo &info = get_info(entry.start);
        const Grid<int> &distances = info.distances;
        int prev_reachable = reachable;
        ++tiles_visited;
        if (entry.distance_left >= info.max_distance) {
            // tile is fully reachable; use pre-calculated values
            if constexpr (aoc::DEBUG && false) {
                if constexpr (!debug) {
                    std::cerr << "  ";
                }
                std::cerr << "tile=" << tile_pos << ": "
                          << ((entry.distance_left % 2 == 0) ? "even" : "odd")
                          << "\n";
            }
            reachable += info.total_reachable[entry.distance_left % 2];
        } else {
            // only part of the tile is reachable, so count the spaces manually
            const int curr_target_distance = entry.distance_left;
            const std::pair<Pos, int> key{entry.start, curr_target_distance};
            auto it = reachable_cache.find(key);
            ++reachable_cache_accesses;
            if (it == reachable_cache.end()) {
                int count = std::count_if(
                    distances.begin(), distances.end(),
                    [&curr_target_distance](int distance) {
                        return distance <= curr_target_distance &&
                               distance % 2 == curr_target_distance % 2;
                    });
                it = reachable_cache.emplace(key, count).first;
            }
            reachable += it->second;
        }
        if constexpr (aoc::DEBUG) {
            std::cerr << "processing " << entry << ": "
                      << (reachable - prev_reachable)
                      << " new reachable positions\n";
        } else {
            (void)prev_reachable;
        }
        const int xpy = tile_pos.x + tile_pos.y;
        const int xmy = tile_pos.x - tile_pos.y;
        const int i = (tile_pos.x & 1) ^ (tile_pos.y & 1);
        if (xpy <= -i && xmy >= -i) {
            dirs.push_back(AbsDirection::north);
        }
        if (xpy >= -i && xmy >= i) {
            dirs.push_back(AbsDirection::east);
        }
        if (xpy >= i && xmy <= i) {
            dirs.push_back(AbsDirection::south);
        }
        if (xpy <= i && xmy <= -i) {
            dirs.push_back(AbsDirection::west);
        }
        for (const AbsDirection &dir : dirs) {
            Delta delta(dir, true);
            Pos new_tile_pos = tile_pos + delta;
            // add new tile, starting from the lowest distance value on the
            // adjacent edge
            // const auto &[min_pos, min_dist] = info.get_edge_pos(dir);
            // const int new_distance = entry.distance_left - (min_dist + 1);
            // if (new_distance >= 0) {
            // Pos new_start = min_pos;
            // new_start.x -= delta.dx * (distances.width - 1);
            // new_start.y -= delta.dy * (distances.height - 1);
            //  add_tile(new_tile_pos, new_start, new_distance);
            add_tile(new_tile_pos);
            //}
        }
        dirs.clear();
    }
    edge_set.clear();
    std::swap(edge_set, next_edge_set);
    if constexpr (debug) {
        std::cerr << "edge set: ";
        for (auto it = edge_set.begin(); it != edge_set.end(); ++it) {
            if (it != edge_set.begin()) {
                std::cerr << ", ";
            }
            std::cerr << *it;
        }
        std::cerr << "\n";
    }
}

long Garden::part_2() const {
    constexpr int target_distance = 26501365; // = 2023 * 100 * 131 + 65
    // constexpr int target_distance = 11;
    // const int target_distance = 2023 * 100 * stones.width + start.x;

    // check our assumptions
    if (!check_part_2()) {
        return -1;
    }

    EdgeSet es(*this, target_distance);
    int i = 0;
    // do the first three iterations to populate dist_info_cache
    for (i = 0; !es.empty() && i <= 3; ++i) {
        if (aoc::DEBUG) {
            std::cerr << "\nstarting iteration " << i << " with " << es.size()
                      << " tiles in edge set...\n";
        }
        es.expand();
    }
    if (!es.empty()) {
        assert(es.dist_info_cache.size() == 9);
        // get the largest of all the maximum_distance values in the cache
        std::array<Pos, 4> midpoint_starts = {
            Pos(start.x, 0),
            Pos(start.x, stones.height - 1),
            Pos(0, start.y),
            Pos(stones.width - 1, start.y),
        };
        std::array<Pos, 4> corner_starts = {
            Pos(0, 0),
            Pos(0, stones.height - 1),
            Pos(stones.width - 1, 0),
            Pos(stones.width - 1, stones.height - 1),
        };
        int max_distance = 0;
        for (const auto &iterable : {midpoint_starts, corner_starts}) {
            for (const Pos &pos : iterable) {
                const auto &info = es.get_info(pos);
                if (info.max_distance > max_distance) {
                    max_distance = info.max_distance;
                }
            }
        }
        long manual_reachable = es.get_reachable();
        // skip to the last fully covered iteration
        int last_full_iter =
            (target_distance - start.x - max_distance) / stones.width - 4;
        {
            std::vector<EdgeSet::DistanceInfo> midpoints;
            std::vector<EdgeSet::DistanceInfo> corners;
            for (std::size_t j = 0; j < 4; ++j) {
                midpoints.push_back(es.get_info(midpoint_starts[j]));
                corners.push_back(es.get_info(corner_starts[j]));
            }
            for (; i < last_full_iter; ++i) {
                /*
                if (aoc::DEBUG) {
                    std::cerr << "\nstarting iteration " << i << " with "
                              << es.size() << " tiles in edge set...\n";
                }
                */
                // es.expand();
                int corner_parity = (i % 2) ^ (target_distance % 2);
                int midpoint_parity = corner_parity ^ (start.x & 1);
                es.tiles_visited += 4 * i;
                for (const auto &info : midpoints) {
                    manual_reachable += info.total_reachable[midpoint_parity];
                }
                for (const auto &info : corners) {
                    manual_reachable += std::max(0, i - 1) *
                                        info.total_reachable[corner_parity];
                }
                // assert(manual_reachable == es.get_reachable());
            }
        }
        // const std::size_t old_size = es.size();
        es.edge_set.clear();
        for (int j = 0; j < i; ++j) {
            const int x = j;
            const int y = i - j;
            es.add_tile(Pos(x, y));
            es.add_tile(Pos(y, -x));
            es.add_tile(Pos(-x, -y));
            es.add_tile(Pos(-y, x));
        }
        std::swap(es.edge_set, es.next_edge_set);
        es.reachable = manual_reachable;
        // assert(es.size() == old_size);
        for (; !es.empty(); ++i) {
            if (aoc::DEBUG) {
                std::cerr << "\nstarting iteration " << i << " with "
                          << es.size() << " tiles in edge set...\n";
            }
            es.expand();
        }
    }
    es.print_stats();
    std::cerr << "took " << i + 1 << " iterations\n";
    long reachable = es.get_reachable();
    if (stones.width == 131) {
        if (target_distance == 2023 * 1 * stones.width + start.x) {
            if (reachable != 59301643058) {
                std::cerr << reachable << "\n";
                assert(false);
            }
            assert(i == 2025);
        } else if (target_distance == 2023 * 10 * stones.width + start.x) {
            if (reachable != 5927505173072) {
                std::cerr << reachable << "\n";
                assert(false);
            }
            assert(i == 20232);
        } else if (target_distance == 2023 * 100 * stones.width + start.x) {
            if (reachable != 592723929260582) {
                std::cerr << reachable << "\n";
                assert(false);
            }
            assert(i == 202302);
        }
    } else if (stones.width == 11) {
        // example3.txt
        if (target_distance == 2023 * 1 * stones.width + start.x) {
            if (reachable != 356242232) {
                std::cerr << reachable << "\n";
                assert(false);
            }
            assert(i == 2025);
        } else if (target_distance == 2023 * 10 * stones.width + start.x) {
            if (reachable != 35606924174) {
                std::cerr << reachable << "\n";
                assert(false);
            }
            assert(i == 20232);
        } else if (target_distance == 2023 * 100 * stones.width + start.x) {
            if (reachable != 3560519448524) {
                std::cerr << reachable << "\n";
                assert(false);
            }
            assert(i == 202302);
        } else if (target_distance == 26501365) {
            if (reachable != 504975595803726) {
                std::cerr << reachable << "\n";
                assert(false);
            }
            assert(i == 2409216);
        }
    } else if (stones.width == 33) {
        // example4.txt
        if (target_distance == 26501365) {
            if (reachable != 504975595803726) {
                std::cerr << reachable << "\n";
                assert(false);
            }
            assert(i == 803073);
        }
    }
    return reachable;
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
