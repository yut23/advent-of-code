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
#include "lib.hpp"             // for Pos, Delta, read_lines, DEBUG
#include "util/hash.hpp"       // for make_hash
#include <algorithm>           // for sort
#include <assert.h>            // for assert
#include <compare>             // for strong_ordering
#include <cstddef>             // for size_t
#include <functional>          // for hash
#include <iomanip>             // for setw
#include <iostream>            // for istream
#include <string>              // for string
#include <utility>             // for pair, move
#include <vector>              // for vector

namespace aoc::day20 {

struct Cheat {
    int time_saved;
    Pos start;
    Pos end;

    std::strong_ordering operator<=>(const Cheat &) const = default;
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
    std::vector<Cheat> find_cheats(int cheat_time_limit) const;
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

std::vector<Cheat> Racetrack::find_cheats(int cheat_time_limit) const {
    const auto process_neighbors = [this](const Pos &pos, auto &&process) {
        grid.manhattan_kernel(pos, [&process](char tile, const Pos &neighbor) {
            if (tile != '#') {
                process(neighbor);
            }
        });
    };
    struct Dists {
        int source_distance;
        int target_distance;

        bool is_wall() const { return source_distance == -1; }
    };
    aoc::ds::Grid<Dists> distances(grid, {-1, -1});
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

    std::vector<Cheat> cheats;
    // for each position on the track, check all the other path tiles reachable
    // within cheat_time_limit picoseconds to see if cheating to them is faster
    distances.for_each([&distances, &cheats, best_uncheated, cheat_time_limit](
                           const Dists &start_dists, const Pos &start) {
        if (start_dists.is_wall()) {
            return;
        }
        // track shouldn't have any dead ends
        assert(start_dists.source_distance + start_dists.target_distance ==
               best_uncheated);

        // check each pair of neighboring tiles and see if they give a
        // smaller total distance than best_uncheated
        distances.manhattan_kernel(
            start, cheat_time_limit,
            [&start, &start_dists, &cheats, best_uncheated](
                const Dists &end_dists, const Pos &end, int path_length) {
                if (end_dists.is_wall()) {
                    return;
                }
                int total_distance = start_dists.source_distance + path_length +
                                     end_dists.target_distance;
                if (total_distance < best_uncheated) {
                    cheats.push_back(
                        {best_uncheated - total_distance, start, end});
                }
            });
    });
    return cheats;
}

int count_cheats(std::vector<Cheat> &cheats, int threshold) {
    int count = 0;
    if constexpr (aoc::DEBUG) {
        std::sort(cheats.begin(), cheats.end());
    }
    for (const auto &cheat : cheats) {
        if (cheat.time_saved >= threshold) {
            if constexpr (aoc::DEBUG) {
                std::cerr << "cheat saves " << std::setw(3) << cheat.time_saved
                          << " picoseconds: " << cheat.start << " to "
                          << cheat.end << "\n";
            }
            ++count;
        }
    }
    return count;
}

Racetrack::Racetrack(std::vector<std::string> &&grid_)
    : grid(std::move(grid_)), start_pos(-1, -1), end_pos(-1, -1) {
    grid.for_each([this](char value, const Pos &pos) {
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
