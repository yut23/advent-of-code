/******************************************************************************
 * File:        day20.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-20
 *****************************************************************************/

#ifndef DAY20_HPP_1NZBO0H2
#define DAY20_HPP_1NZBO0H2

#include "ds/grid.hpp"         // for Grid
#include "graph_traversal.hpp" // for bfs
#include "lib.hpp"             // for Pos, Delta, DIRECTIONS, read_lines, DEBUG
#include "util/hash.hpp"       // for make_hash
#include <assert.h>            // for assert
#include <compare>             // for strong_ordering
#include <cstddef>             // for size_t
#include <functional>          // for hash
#include <initializer_list>    // for initializer_list
#include <iostream>            // for istream
#include <string>              // for string
#include <utility>             // for pair, move
#include <vector>              // for vector

namespace aoc::day20 {

struct Cheat {
    Pos start;
    Pos end;
    int time_saved;

    bool operator==(const Cheat &) const = default;
};

class Racetrack {
  public:
    struct Key {
        Pos pos;
        bool cheated;

        std::strong_ordering operator<=>(const Key &) const = default;
    };

  private:
    aoc::ds::Grid<char> grid;
    Pos start_pos;
    Pos end_pos;

    int best_path_length(bool cheat) const;

  public:
    std::vector<Cheat> find_cheats() const;
    void print(std::ostream &, const std::vector<Key> &path = {}) const;

    explicit Racetrack(std::vector<std::string> &&grid_);
    static Racetrack read(std::istream &is) {
        return Racetrack{aoc::read_lines(is)};
    }
};

} // namespace aoc::day20

template <>
struct std::hash<aoc::day20::Racetrack::Key> {
    std::size_t
    operator()(const aoc::day20::Racetrack::Key &key) const noexcept {
        // random number (hexdump -n8 -e '"0x" 8/1 "%02x" "ull\n"'</dev/urandom)
        std::size_t seed = 0xc761224f8b32b561ull;
        util::make_hash(seed, key.pos.x, key.pos.y, key.cheated);
        return seed;
    }
};

namespace aoc::day20 {

std::vector<Cheat> Racetrack::find_cheats() const {
    const auto process_neighbors = [this](const Pos &pos, auto &&process) {
        for (auto dir : DIRECTIONS) {
            Pos neighbor = pos + Delta(dir, true);
            if (grid.in_bounds(neighbor) && grid[neighbor] != '#') {
                process(neighbor);
            }
        }
    };
    struct Dists {
        int source_distance = -1;
        int target_distance = -1;
    };
    aoc::ds::Grid<Dists> distances(grid, {});
    // get the distance from the start to each position without cheating
    aoc::graph::bfs(start_pos, process_neighbors, {},
                    [&distances](const Pos &pos, int depth) {
                        // bfs visits each position once, with the shortest
                        // distance
                        distances[pos].source_distance = depth;
                    });
    // get the distance from each position to the end position without cheating
    aoc::graph::bfs(end_pos, process_neighbors, {},
                    [&distances](const Pos &pos, int depth) {
                        distances[pos].target_distance = depth;
                    });

    int best_uncheated = distances[end_pos].source_distance;
    if constexpr (aoc::DEBUG) {
        std::cerr << "best uncheated distance: " << best_uncheated << "\n";
    }

    // try cheating through each wall
    std::vector<Cheat> cheats;
    grid.for_each_with_pos(
        [&distances, &cheats, best_uncheated](const Pos &pos, char value) {
            if (value != '#') {
                return;
            }
            // enumerate neighboring non-wall positions
            std::vector<std::pair<Dists, Pos>> neighbors;
            for (auto dir : DIRECTIONS) {
                Pos neighbor = pos + Delta(dir, true);
                if (!distances.in_bounds(neighbor)) {
                    continue;
                }
                Dists &dists = distances[neighbor];
                if (dists.source_distance != -1) {
                    neighbors.push_back({dists, neighbor});
                }
            }

            // check each pair of neighboring tiles and see if they give a
            // smaller total distance than best_uncheated
            for (std::size_t i = 0; i < neighbors.size(); ++i) {
                for (std::size_t j = 0; j < neighbors.size(); j++) {
                    if (i == j) {
                        continue;
                    }
                    int total_distance = neighbors[i].first.source_distance +
                                         2 + neighbors[j].first.target_distance;
                    if (total_distance < best_uncheated) {
                        cheats.push_back({pos, neighbors[i].second,
                                          best_uncheated - total_distance});
                    }
                }
            }
        });
    return cheats;
}

Racetrack::Racetrack(std::vector<std::string> &&grid_)
    : grid(std::move(grid_)), start_pos(-1, -1), end_pos(-1, -1) {
    grid.for_each_with_pos([this](const Pos &pos, char value) {
        if (value == 'S') {
            this->start_pos = pos;
        } else if (value == 'E') {
            this->end_pos = pos;
        }
    });
    assert(start_pos.x != -1 && start_pos.y != -1);
    assert(end_pos.x != -1 && end_pos.y != -1);
}

} // namespace aoc::day20

#endif /* end of include guard: DAY20_HPP_1NZBO0H2 */
