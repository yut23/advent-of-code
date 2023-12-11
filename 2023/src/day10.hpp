/******************************************************************************
 * File:        day10.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-10
 *****************************************************************************/

#include "lib.hpp"   // for Direction, Pos, Delta, DEBUG
#include <algorithm> // for transform
#include <cassert>   // for assert
#include <cstddef>   // for size_t
#include <iostream>  // for cerr, istream
#include <iterator>  // for back_inserter
#include <optional>  // for optional
#include <string>    // for string, getline
#include <utility>   // for move
#include <vector>    // for vector

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

std::optional<Direction> get_out_dir(Pipe pipe, Direction in_dir) {
    switch (pipe) {
    case Pipe::straight_vert:
        if (in_dir == Direction::up || in_dir == Direction::down) {
            return in_dir;
        }
        break;
    case Pipe::straight_horz:
        if (in_dir == Direction::left || in_dir == Direction::right) {
            return in_dir;
        }
        break;
    case Pipe::bend_ne:
        if (in_dir == Direction::down) {
            return Direction::right;
        }
        if (in_dir == Direction::left) {
            return Direction::up;
        }
        break;
    case Pipe::bend_nw:
        if (in_dir == Direction::down) {
            return Direction::left;
        }
        if (in_dir == Direction::right) {
            return Direction::up;
        }
        break;
    case Pipe::bend_sw:
        if (in_dir == Direction::up) {
            return Direction::left;
        }
        if (in_dir == Direction::right) {
            return Direction::down;
        }
        break;
    case Pipe::bend_se:
        if (in_dir == Direction::up) {
            return Direction::right;
        }
        if (in_dir == Direction::left) {
            return Direction::down;
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

struct PipeGrid {
    std::vector<std::vector<Pipe>> pipes;
    Pos start_pos{-1, -1};

    void find_start() {
        for (std::size_t y = 0; y < pipes.size(); ++y) {
            for (std::size_t x = 0; x < pipes[y].size(); ++x) {
                Pos pos{int(x), int(y)};
                if (at(pos) == Pipe::start) {
                    start_pos = pos;
                    return;
                }
            }
        }
    }

  public:
    explicit PipeGrid(const std::vector<std::vector<Pipe>> &pipes)
        : pipes(pipes) {
        find_start();
    }
    explicit PipeGrid(std::vector<std::vector<Pipe>> &&pipes)
        : pipes(std::move(pipes)) {
        find_start();
    }

    Pipe operator[](const Pos &pos) const { return pipes[pos.y][pos.x]; }
    Pipe at(const Pos &pos) const { return pipes.at(pos.y).at(pos.x); }

    PipeIterator begin() const;

    bool operator==(const PipeGrid &) const = default;
};

struct PipeIterator {
    const PipeGrid *grid;
    Pos pos;
    Direction next_dir;

    PipeIterator(const PipeGrid &grid, const Pos &pos,
                 const Direction &next_dir)
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
        std::optional<Direction> dir = get_out_dir(grid->at(pos), next_dir);
        assert(dir.has_value());
        next_dir = *dir;
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

PipeIterator PipeGrid::begin() const {
    for (Direction dir :
         {Direction::up, Direction::down, Direction::left, Direction::right}) {
        Pipe neighbor = at(start_pos + Delta(dir, true));
        std::optional<Direction> out_dir = get_out_dir(neighbor, dir);
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
    return PipeGrid(std::move(pipes));
}

} // namespace aoc::day10
