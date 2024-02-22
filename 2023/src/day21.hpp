/******************************************************************************
 * File:        day21.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-21
 *****************************************************************************/

#ifndef DAY21_HPP_2X5T9V1Y
#define DAY21_HPP_2X5T9V1Y

#include "ds/grid.hpp"         // for Grid
#include "graph_traversal.hpp" // for bfs
#include "lib.hpp"          // for AbsDirection, Pos, Delta, DIRECTIONS, DEBUG
#include <array>            // for array
#include <cassert>          // for assert
#include <compare>          // for strong_ordering
#include <cstdlib>          // for abs, size_t
#include <initializer_list> // for initializer_list
#include <iostream>         // for istream, noskipws, cerr
#include <limits>           // for numeric_limits
#include <vector>           // for vector
// IWYU pragma: no_include <algorithm>  // for copy

namespace aoc::day21 {

using aoc::ds::Grid;

struct Garden {
    using Key = aoc::Pos;

    const Pos start;
    const Grid<bool> stones;

    Garden(const Pos &start, const std::vector<std::vector<bool>> &grid)
        : start(start), stones(grid) {
        assert(stones.in_bounds(start));
    }

    Grid<int> get_distances(const Key &source) const {
        Grid<int> distances(stones, std::numeric_limits<int>::max());
        distances[source] = 0;

        const auto process_neighbors = [this, &distances](const Key &key,
                                                          auto &&visit) {
            for (const AbsDirection &dir : aoc::DIRECTIONS) {
                Pos pos = key + Delta(dir, true);
                if (stones.in_bounds(pos) && stones[pos] &&
                    distances[pos] == std::numeric_limits<int>::max()) {
                    visit(pos);
                }
            }
        };
        const auto visit = [&distances](const Key &key, int distance) {
            distances[key] = distance;
        };

        // handled in get_neighbors
        constexpr bool use_seen = false;
        aoc::graph::bfs<use_seen>(source, process_neighbors, visit);

        return distances;
    }

    bool check_part_2() const;

  public:
    int part_1() const;
    long part_2() const;
    static Garden read(std::istream &is);
};

long count_reachable_plots(const Grid<int> &distances,
                           const int target_distance) {
    const int parity = target_distance % 2;
    long count = 0;
    for (const int distance : distances.data()) {
        if (distance <= target_distance && distance % 2 == parity) {
            // this is faster under ASan
            // cppcheck-suppress useStlAlgorithm
            ++count;
        }
    }
    return count;
}

int Garden::part_1() const {
    constexpr int target_distance = 64;

    const Grid<int> distances = get_distances(start);

    return count_reachable_plots(distances, target_distance);
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
        Pos start;
        int distance_left;

        std::strong_ordering operator<=>(const Entry &) const = default;
    };
    struct DistanceInfo {
        Grid<int> distances;
        int max_distance = 0;
        std::array<int, 2> total_reachable{0, 0};

        explicit DistanceInfo(const Grid<int> &distances_)
            : distances(distances_) {
            for (int distance : distances.data()) {
                if (distance == std::numeric_limits<int>::max()) {
                    continue;
                }
                if (distance > max_distance) {
                    max_distance = distance;
                }
                ++total_reachable[distance % 2];
            }
        }
    };

    const Garden &garden;
    const int target_distance;

    int iter = 0;
    int last_full_iter = 0;
    bool _made_progress = true;

    long reachable = 0;
    long tiles_visited = 0;

    Entry make_entry(const Pos &tile_pos) const;

    DistanceInfo get_info(const Pos &start) {
        return DistanceInfo(garden.get_distances(start));
    }

    int get_reachable_for_tile(const Pos &tile_pos,
                               const DistanceInfo &info) const;

  public:
    explicit EdgeSet(const Garden &garden_, int target_distance_)
        : garden(garden_), target_distance(target_distance_) {}

    void expand(int max_iter = std::numeric_limits<int>::max());

    bool done() const { return !_made_progress; }

    long get_reachable() const { return reachable; }
    int get_iter() const { return iter; }

    void print_stats() const {
        std::cerr << "visited " << tiles_visited << " tiles\n"
                  << "took " << iter << " iterations\n";
    }

    friend std::ostream &operator<<(std::ostream &, const Entry &);
};

std::ostream &operator<<(std::ostream &os, const EdgeSet::Entry &entry) {
    os << "Entry(start=(" << entry.start.x << ", " << entry.start.y
       << "), distance_left=" << entry.distance_left << ")";
    return os;
}

EdgeSet::Entry EdgeSet::make_entry(const Pos &tile_pos) const {
    // start from the corner/edge closest to the original starting point
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

    return Entry{start, distance_left};
}

int EdgeSet::get_reachable_for_tile(const Pos &tile_pos,
                                    const DistanceInfo &info) const {
    if (iter < last_full_iter) {
        int parity = (iter % 2) ^ (target_distance % 2);
        if ((tile_pos.x == 0) ^ (tile_pos.y == 0)) {
            parity ^= (garden.start.x & 1);
        }
        return info.total_reachable[parity];
    }
    const Entry entry = make_entry(tile_pos);
    if (entry.distance_left < 0) {
        return 0;
    }
    int curr_reachable;
    if (entry.distance_left >= info.max_distance) {
        // tile is fully reachable; use pre-calculated values
        curr_reachable = info.total_reachable[entry.distance_left % 2];
    } else {
        // only part of the tile is reachable, so count the spaces manually
        curr_reachable =
            count_reachable_plots(info.distances, entry.distance_left);
    }
    if constexpr (aoc::DEBUG) {
        std::cerr << "processed " << entry << " @ " << tile_pos << ": "
                  << curr_reachable << " new reachable positions\n";
    }
    return curr_reachable;
}

void EdgeSet::expand(int max_iter) {
    // std::map lookups are the current bottleneck, so only look these up once
    const DistanceInfo &south = get_info(Pos(garden.start.x, 0));
    const DistanceInfo &east = get_info(Pos(0, garden.start.y));
    const DistanceInfo &north =
        get_info(Pos(garden.start.x, garden.stones.height - 1));
    const DistanceInfo &west =
        get_info(Pos(garden.stones.width - 1, garden.start.y));

    const DistanceInfo &south_east = get_info(Pos(0, 0));
    const DistanceInfo &north_east = get_info(Pos(0, garden.stones.height - 1));
    const DistanceInfo &north_west =
        get_info(Pos(garden.stones.width - 1, garden.stones.height - 1));
    const DistanceInfo &south_west = get_info(Pos(garden.stones.width - 1, 0));

    // speed up get_reachable_for_tile() for iterations where all tiles are
    // fully covered
    int max_distance = std::max(
        {south.max_distance, east.max_distance, north.max_distance,
         west.max_distance, south_east.max_distance, north_east.max_distance,
         north_west.max_distance, south_west.max_distance});
    last_full_iter = (target_distance - garden.start.x - max_distance) /
                         garden.stones.width -
                     1;

    for (; _made_progress && iter < max_iter; ++iter) {
        long prev_reachable = reachable;
        if constexpr (aoc::DEBUG) {
            if (iter % 1000 == 1) {
                std::cerr << "\nstarting iteration " << iter << " with "
                          << iter * 4 << " tiles in edge set...\n";
            }
        }
        if (iter == 0) {
            reachable +=
                get_reachable_for_tile(Pos(0, 0), get_info(garden.start));
            ++tiles_visited;
        } else {
            // on-axis tiles
            reachable += get_reachable_for_tile(Pos(0, iter), south);
            reachable += get_reachable_for_tile(Pos(iter, 0), east);
            reachable += get_reachable_for_tile(Pos(0, -iter), north);
            reachable += get_reachable_for_tile(Pos(-iter, 0), west);
            tiles_visited += 4;
            // off-axis tiles
            if (iter > 1) {
                // The starting point is at the center of the tile and the tiles
                // are square, so the distance to each off-axis tile will be the
                // same and we only have to calculate the reachable tiles once
                // for each direction.
                const int x = 1;
                const int y = iter - 1;
                reachable +=
                    (iter - 1) * get_reachable_for_tile(Pos(x, y), south_east);
                reachable +=
                    (iter - 1) * get_reachable_for_tile(Pos(y, -x), north_east);
                reachable += (iter - 1) *
                             get_reachable_for_tile(Pos(-x, -y), north_west);
                reachable +=
                    (iter - 1) * get_reachable_for_tile(Pos(-y, x), south_west);
                tiles_visited += 4 * (iter - 1);
            }
        }
        _made_progress = reachable != prev_reachable;
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
    es.expand();

    if constexpr (aoc::DEBUG) {
        es.print_stats();
    }
    long reachable = es.get_reachable();
#if 0
    int i = es.get_iter();
    if (stones.width == 131) {
        if (target_distance == 2023 * 1 * stones.width + start.x) {
            assert(i >= 2025);
            if (reachable != 59301643058) {
                std::cerr << reachable << "\n";
                assert(false);
            }
        } else if (target_distance == 2023 * 10 * stones.width + start.x) {
            assert(i >= 20232);
            if (reachable != 5927505173072) {
                std::cerr << reachable << "\n";
                assert(false);
            }
        } else if (target_distance == 2023 * 100 * stones.width + start.x) {
            assert(i >= 202302);
            if (reachable != 592723929260582) {
                std::cerr << reachable << "\n";
                assert(false);
            }
        }
    } else if (stones.width == 11) {
        // example3.txt
        if (target_distance == 2023 * 1 * stones.width + start.x) {
            assert(i >= 2025);
            if (reachable != 356242232) {
                std::cerr << reachable << "\n";
                assert(false);
            }
        } else if (target_distance == 2023 * 10 * stones.width + start.x) {
            assert(i >= 20232);
            if (reachable != 35606924174) {
                std::cerr << reachable << "\n";
                assert(false);
            }
        } else if (target_distance == 2023 * 100 * stones.width + start.x) {
            assert(i >= 202302);
            if (reachable != 3560519448524) {
                std::cerr << reachable << "\n";
                assert(false);
            }
            // cppcheck-suppress knownConditionTrueFalse
        } else if (target_distance == 26501365) {
            assert(i >= 2409216);
            if (reachable != 504975595803726) {
                std::cerr << reachable << "\n";
                assert(false);
            }
        }
    } else if (stones.width == 33) {
        // example4.txt
        // cppcheck-suppress knownConditionTrueFalse
        if (target_distance == 26501365) {
            assert(i >= 803073);
            if (reachable != 504975595803726) {
                std::cerr << reachable << "\n";
                assert(false);
            }
        }
    }
#endif
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
