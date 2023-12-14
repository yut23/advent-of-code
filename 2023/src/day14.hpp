/******************************************************************************
 * File:        day14.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-14
 *****************************************************************************/

#include "lib.hpp"  // for Pos, Delta, Direction
#include <cstddef>  // for size_t
#include <iostream> // for istream
#include <string>   // for string, getline
#include <vector>   // for vector

namespace aoc::day14 {

enum class Rock : char {
    empty = '.',
    round = 'O',
    cube = '#',
};

struct Platform {
    std::vector<Pos> round_rocks;
    std::vector<std::vector<Rock>> grid;

  private:
    bool is_in_bounds(const Pos &pos) {
        return pos.x >= 0 && pos.y >= 0 &&
               static_cast<std::size_t>(pos.y) < grid.size() &&
               static_cast<std::size_t>(pos.x) < grid[pos.y].size();
    }
    bool move_rock(std::size_t index, const Delta &delta);
    Rock &operator[](const Pos &pos) { return grid[pos.y][pos.x]; }

  public:
    void add_rock(const Pos &pos, Rock rock);
    void tilt(Direction dir);
    int calculate_load() const;
    Rock operator[](const Pos &pos) const { return grid[pos.y][pos.x]; }
};

bool Platform::move_rock(std::size_t index, const Delta &delta) {
    Pos &pos = round_rocks[index];
    Pos new_pos = pos;
    Pos last_good_pos = pos;
    while (true) {
        new_pos += delta;
        if (!is_in_bounds(new_pos)) {
            break;
        }
        Rock rock = (*this)[new_pos];
        if (rock == Rock::cube) {
            break;
        } else if (rock == Rock::round) {
            // try skipping over round rocks
            continue;
        } else if (rock == Rock::empty) {
            last_good_pos = new_pos;
        }
    }
    if (last_good_pos != pos) {
        (*this)[pos] = Rock::empty;
        pos = last_good_pos;
        (*this)[pos] = Rock::round;
        return true;
    }
    return false;
}

void Platform::add_rock(const Pos &pos, Rock rock) {
    if (rock == Rock::round) {
        round_rocks.push_back(pos);
    }
    if (static_cast<std::size_t>(pos.y) >= grid.size()) {
        grid.resize(pos.y + 1);
    }
    if (static_cast<std::size_t>(pos.x) >= grid[pos.y].size()) {
        for (auto &row : grid) {
            if (static_cast<std::size_t>(pos.x) >= row.size()) {
                row.resize(pos.x + 1, Rock::empty);
            }
        }
    }
    (*this)[pos] = rock;
}

void Platform::tilt(Direction dir) {
    Delta delta{dir, true};
    // most naive method: bubble sort
    for (std::size_t i = 0; i < round_rocks.size(); ++i) {
        move_rock(i, delta);
    }
}

int Platform::calculate_load() const {
    int load = 0;
    for (const Pos &pos : round_rocks) {
        load += grid.size() - pos.y;
    }
    return load;
}

std::ostream &operator<<(std::ostream &os, const Platform &platform) {
    for (const auto &row : platform.grid) {
        for (Rock rock : row) {
            os << static_cast<char>(rock);
        }
        os << "\n";
    }

    return os;
}

Platform read_platform(std::istream &is) {
    Platform platform{};
    std::string line;
    Pos pos;
    for (pos.y = 0; std::getline(is, line); ++pos.y) {
        for (pos.x = 0; pos.x < static_cast<int>(line.size()); ++pos.x) {
            Rock rock{line[pos.x]};
            platform.add_rock(pos, rock);
        }
    }
    return platform;
}

} // namespace aoc::day14
