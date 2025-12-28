/******************************************************************************
 * File:        day20.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-20
 *****************************************************************************/

#ifndef DAY20_HPP_1NZBO0H2
#define DAY20_HPP_1NZBO0H2

#include "ds/grid.hpp"      // for Grid
#include "lib.hpp"          // for Pos, Delta, read_lines, DIRECTIONS, DEBUG
#include <algorithm>        // for sort
#include <cassert>          // for assert
#include <compare>          // for strong_ordering
#include <cstddef>          // for size_t
#include <initializer_list> // for initializer_list (DIRECTIONS)
#include <iostream>         // for istream, ostream, cerr
#include <string>           // for string (read_lines)
#include <utility>          // for pair
#include <vector>           // for vector

namespace aoc::day20 {

// only used for sorting debug output
struct Cheat {
    int time_saved;
    Pos start;
    Pos end;
    int length;

    std::strong_ordering operator<=>(const Cheat &) const = default;
};

std::ostream &operator<<(std::ostream &os, const Cheat &cheat) {
    os << "cheat saves " << cheat.time_saved << " ps: " << cheat.start << " to "
       << cheat.end << ", length=" << cheat.length;
    return os;
}

class Racetrack {
    std::vector<std::pair<Pos, int>> race_path{};
    Pos start_pos{-1, -1};
    Pos end_pos{-1, -1};

    void trace_path(aoc::ds::Grid<char> &grid);

  public:
    std::pair<int, int> count_cheats(int cheat_time_limit_1,
                                     int cheat_time_limit_2,
                                     int threshold) const;

    static Racetrack read(std::istream &is);
};

std::pair<int, int> Racetrack::count_cheats(int cheat_time_limit_1,
                                            int cheat_time_limit_2,
                                            int threshold) const {
    int best_uncheated = race_path.back().second;
    if constexpr (aoc::DEBUG) {
        std::cerr << "best uncheated distance: " << best_uncheated << "\n";
    }
    // keep track of the cheats so we can sort them in the debug output
    std::vector<Cheat> part_1_cheats;
    std::vector<Cheat> part_2_cheats;

    int count_1 = 0, count_2 = 0;
    // check each pair of tiles on the path and see if cheating between them
    // saves more than the threshold
    for (std::size_t i = 0; i < race_path.size(); ++i) {
        const auto &[start, start_dist] = race_path[i];
        // in order to save more than the threshold, we have to go at least
        // this far along the race path (see input/day20/example2.txt)
        for (std::size_t j = i + threshold + cheat_time_limit_1;
             j < race_path.size(); ++j) {
            const auto &[end, end_dist] = race_path[j];
            // distance from cheat end point to the race end point is
            // (best_uncheated - end_dist)
            int path_length = (start - end).manhattan_distance();
            if (path_length - cheat_time_limit_2 > 0) {
                // We can't reach the cheat end point within the time limit,
                // and it would have to move directly toward the cheat start
                // point by (path_length - cheat_time_limit_2) tiles in order
                // to be within range for part 2, so we can safely skip that
                // far forward in the race path (minus 1 to account for the ++j
                // from the for loop).
                j += path_length - cheat_time_limit_2 - 1;
                continue;
            }
            // this is equivalent to start_dist + path_length + (best_uncheated
            // - end_dist) < best_uncheated - threshold
            if (end_dist - start_dist - path_length >= threshold) {
                if constexpr (aoc::DEBUG) {
                    part_2_cheats.push_back(
                        {end_dist - start_dist - path_length, start, end,
                         path_length});
                }
                if (path_length <= cheat_time_limit_1) {
                    ++count_1;
                    if constexpr (aoc::DEBUG) {
                        part_1_cheats.push_back(part_2_cheats.back());
                    }
                }
                ++count_2;
            }
        }
    }
    if constexpr (aoc::DEBUG) {
        std::sort(part_1_cheats.begin(), part_1_cheats.end());
        std::cerr << "Part 1, time limit " << cheat_time_limit_1
                  << " ps: " << part_1_cheats.size() << " cheats\n";
        for (const auto &cheat : part_1_cheats) {
            std::cerr << cheat << "\n";
        }

        std::sort(part_2_cheats.begin(), part_2_cheats.end());
        std::cerr << "\nPart 2, time limit " << cheat_time_limit_2
                  << " ps: " << part_2_cheats.size() << " cheats\n";
        for (const auto &cheat : part_2_cheats) {
            std::cerr << cheat << "\n";
        }
    }

    return {count_1, count_2};
}

Racetrack Racetrack::read(std::istream &is) {
    aoc::ds::Grid<char> grid(aoc::read_lines(is));

    Racetrack track{};
    grid.for_each([&track](char value, const Pos &pos) {
        if (value == 'S') {
            track.start_pos = pos;
        } else if (value == 'E') {
            track.end_pos = pos;
        }
    });
    assert(track.start_pos.x != -1 && track.start_pos.y != -1);
    assert(track.end_pos.x != -1 && track.end_pos.y != -1);

    track.trace_path(grid);
    return track;
}

void Racetrack::trace_path(aoc::ds::Grid<char> &grid) {
    // find the race path
    Pos pos = start_pos;
    int source_distance = 0;
    while (true) {
        race_path.emplace_back(pos, source_distance);
        ++source_distance;
        if (pos == end_pos) {
            break;
        }
        bool found_path = false;
        Pos next_pos;
        for (const AbsDirection dir : DIRECTIONS) {
            Pos new_pos = pos + Delta(dir, true);
            if (!grid.in_bounds(new_pos) || grid[new_pos] == '#') {
                continue;
            }
            if (found_path) {
                std::cerr << "error: racetrack branches at " << pos << "\n";
                assert(false);
            }
            // block off the path back
            grid[pos] = '#';
            found_path = true;
            next_pos = new_pos;
        }
        assert(found_path);
        pos = next_pos;
    }
}

} // namespace aoc::day20

#endif /* end of include guard: DAY20_HPP_1NZBO0H2 */
