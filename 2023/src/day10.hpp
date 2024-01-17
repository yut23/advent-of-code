/******************************************************************************
 * File:        day10.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-10
 *****************************************************************************/

#ifndef DAY10_HPP_CJA1L45J
#define DAY10_HPP_CJA1L45J

#include "data_structures.hpp" // for Grid
#include "lib.hpp"           // for AbsDirection, Pos, Delta, DEBUG, DIRECTIONS
#include "util/concepts.hpp" // for const_or_rvalue_ref

#include <algorithm>        // for transform, find
#include <cassert>          // for assert
#include <initializer_list> // for initializer_list
#include <iostream>         // for cerr, istream
#include <iterator>         // for back_inserter, distance
#include <optional>         // for optional
#include <string>           // for string, getline
#include <utility>          // for move, forward
#include <vector>           // for vector
// IWYU pragma: no_include <functional>  // for identity (ranges::transform)

namespace aoc::day10 {

enum class Pipe : char {
    ground = '.',
    straight_vert = '|',
    straight_horz = '-',
    bend_ne = 'L',
    bend_nw = 'J',
    bend_sw = '7',
    bend_se = 'F',
    start = 'S',
};

std::optional<AbsDirection> get_out_dir(Pipe pipe, AbsDirection in_dir) {
    switch (pipe) {
    case Pipe::straight_vert:
        if (in_dir == AbsDirection::north || in_dir == AbsDirection::south) {
            return in_dir;
        }
        break;
    case Pipe::straight_horz:
        if (in_dir == AbsDirection::west || in_dir == AbsDirection::east) {
            return in_dir;
        }
        break;
    case Pipe::bend_ne:
        if (in_dir == AbsDirection::south) {
            return AbsDirection::east;
        }
        if (in_dir == AbsDirection::west) {
            return AbsDirection::north;
        }
        break;
    case Pipe::bend_nw:
        if (in_dir == AbsDirection::south) {
            return AbsDirection::west;
        }
        if (in_dir == AbsDirection::east) {
            return AbsDirection::north;
        }
        break;
    case Pipe::bend_sw:
        if (in_dir == AbsDirection::north) {
            return AbsDirection::west;
        }
        if (in_dir == AbsDirection::east) {
            return AbsDirection::south;
        }
        break;
    case Pipe::bend_se:
        if (in_dir == AbsDirection::north) {
            return AbsDirection::east;
        }
        if (in_dir == AbsDirection::west) {
            return AbsDirection::south;
        }
        break;
    case Pipe::start:
        return in_dir;
    case Pipe::ground:
        break;
    }
    return {};
}

struct PipeIterator;

struct PipeGrid : public aoc::ds::Grid<Pipe> {
    Pos start_pos{-1, -1};

    template <
        util::concepts::const_or_rvalue_ref<std::vector<std::vector<Pipe>>> V>
    explicit PipeGrid(V &&pipes) : Grid(std::forward<V>(pipes)) {
        start_pos = index_to_pos(
            std::distance(cbegin(), std::find(cbegin(), cend(), Pipe::start)));
    }

    PipeIterator pipe_iterator() const;
};

struct PipeIterator {
    const PipeGrid *grid;
    Pos pos;
    AbsDirection next_dir;

    PipeIterator(const PipeGrid &grid, const Pos &pos,
                 const AbsDirection &next_dir)
        : grid(&grid), pos(pos), next_dir(next_dir) {}

  public:
    PipeIterator(const PipeIterator &other) = default;
    PipeIterator(PipeIterator &&other) = default;

    PipeIterator &operator++() {
        if constexpr (aoc::DEBUG) {
            std::cerr << "moving " << next_dir << " from " << pos << " to ";
        }
        pos += Delta(next_dir, true);
        if constexpr (aoc::DEBUG) {
            std::cerr << pos << "\n";
        }
        next_dir = *get_out_dir(grid->at(pos), next_dir);
        return *this;
    }

    PipeIterator operator++(int) {
        PipeIterator orig = *this;
        ++(*this);
        return orig;
    }

    Pos operator*() { return pos; }

    bool operator==(const PipeIterator &) const = default;

    friend PipeGrid;
};

PipeIterator PipeGrid::pipe_iterator() const {
    for (AbsDirection dir : aoc::DIRECTIONS) {
        Pos pos = start_pos + Delta(dir, true);
        if (!in_bounds(pos)) {
            continue;
        }
        std::optional<AbsDirection> out_dir = get_out_dir(at(pos), dir);
        if (out_dir) {
            return PipeIterator(*this, start_pos, dir);
        }
    }
    assert(false);
}

PipeGrid read_pipes(std::istream &is) {
    // read file line-by-line
    std::vector<std::vector<Pipe>> pipes;
    std::string line;
    while (std::getline(is, line)) {
        std::vector<Pipe> pipe_line;
        std::ranges::transform(line, std::back_inserter(pipe_line),
                               [](const char &ch) { return Pipe{ch}; });
        pipes.push_back(std::move(pipe_line));
    }
    return PipeGrid{std::move(pipes)};
}

} // namespace aoc::day10

#endif /* end of include guard: DAY10_HPP_CJA1L45J */
