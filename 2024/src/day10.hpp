/******************************************************************************
 * File:        day10.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-10
 *****************************************************************************/

#ifndef DAY10_HPP_JPHYNOME
#define DAY10_HPP_JPHYNOME

#include "ds/grid.hpp"         // for Grid
#include "graph_traversal.hpp" // for bfs
#include "lib.hpp"             // for Pos, as_number, DEBUG, DIRECTIONS, Delta

#include <algorithm> // for transform
#include <cstddef>   // for size_t
#include <cstdint>   // for int8_t
#include <iostream>  // for istream, ostream, cerr
#include <iterator>  // for back_inserter
#include <string>    // for string, getline
#include <utility>   // for move
#include <vector>    // for vector
// IWYU pragma: no_include <initializer_list>  // for aoc::DIRECTIONS

namespace aoc::day10 {

using height_t = std::uint8_t;
constexpr height_t IMPASSABLE = 11;

class IslandMap : public aoc::ds::Grid<height_t> {
    std::vector<Pos> trailheads{};
    std::vector<Pos> peaks{};

    explicit IslandMap(std::vector<std::vector<height_t>> &&height_map);
    void process_neighbors(const Pos &key, auto &&process) const;
    template <aoc::Part part>
    aoc::ds::Grid<int> calc_scores() const;

  public:
    static IslandMap read(std::istream &is);
    template <aoc::Part part>
    int trailhead_scores() const;
};

IslandMap IslandMap::read(std::istream &is) {
    std::vector<std::vector<height_t>> height_map;
    std::string line;
    while (std::getline(is, line)) {
        std::vector<height_t> row;
        std::transform(
            line.begin(), line.end(), std::back_inserter(row),
            [](char ch) { return ch == '.' ? IMPASSABLE : ch - '0'; });
        height_map.push_back(std::move(row));
    }
    return IslandMap{std::move(height_map)};
}

IslandMap::IslandMap(std::vector<std::vector<height_t>> &&height_map)
    : aoc::ds::Grid<height_t>(std::move(height_map)) {
    // identify trailheads and peaks
    for (std::size_t i = 0; i < m_data.size(); ++i) {
        if (m_data[i] == 0) {
            trailheads.push_back(index_to_pos(i));
        } else if (m_data[i] == 9) {
            peaks.push_back(index_to_pos(i));
        }
    }
}

std::ostream &operator<<(std::ostream &os, const IslandMap &island) {
    for (const auto &row : island) {
        for (const auto tile : row) {
            if (tile == IMPASSABLE) {
                os << '.';
            } else {
                os << aoc::as_number(tile);
            }
        }
        os << '\n';
    }
    return os;
}

/**
 * Calculate scores using DP, by walking down from each peak and incrementing
 * each visited tile's score by 1.
 */
template <aoc::Part part>
aoc::ds::Grid<int> IslandMap::calc_scores() const {
    // initialize all scores to 0
    aoc::ds::Grid<int> scores(width, height, 0);

    for (const Pos &peak : peaks) {
        const auto visit_with_parent =
            [&scores](const Pos &key, const Pos & /*parent*/, int /*depth*/) {
                ++scores[key];
            };
        // only visit each tile once for part 1
        constexpr bool use_seen = part == PART_1;
        aoc::graph::dfs_rec<use_seen>(
            peak,
            [this](const Pos &pos, auto &&process) {
                this->process_neighbors(pos, process);
            },
            /*is_target*/ {}, visit_with_parent);
    }

    return scores;
}

// walk down from peaks to trailheads
void IslandMap::process_neighbors(const Pos &pos, auto &&process) const {
    height_t curr_height = at(pos);
    if (curr_height == 0) {
        return;
    }
    // loop over cardinal directions
    for (const auto &dir : DIRECTIONS) {
        Pos neighbor = pos + Delta(dir, true);
        if (in_bounds(neighbor) && (*this)[neighbor] == curr_height - 1) {
            process(neighbor);
        }
    }
}

template <aoc::Part part>
int IslandMap::trailhead_scores() const {
    // calculate the scores for the entire map, then look up the trailheads
    const auto scores = calc_scores<part>();

    if constexpr (aoc::DEBUG) {
        std::cerr << "map:\n" << *this << "\n";
        std::cerr << "scores:\n";
        for (const auto &row : scores) {
            for (const auto score : row) {
                if (score == 0) {
                    std::cerr << '.';
                } else {
                    std::cerr << score;
                }
            }
            std::cerr << '\n';
        }
    }
    int total_score = 0;
    for (const Pos &pos : trailheads) {
        total_score += scores[pos];
    }
    return total_score;
}

} // namespace aoc::day10

#endif /* end of include guard: DAY10_HPP_JPHYNOME */
