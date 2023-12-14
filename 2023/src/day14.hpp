/******************************************************************************
 * File:        day14.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-14
 *****************************************************************************/

#include "lib.hpp"  // for Pos, Delta, Direction
#include <cstddef>  // for size_t
#include <iostream> // for istream
#include <set>      // for set
#include <string>   // for string, getline
#include <vector>   // for vector

namespace aoc::day14 {

struct Platform {
    std::vector<Pos> round_rocks;
    std::set<Pos> round_rock_lookup;
    std::set<Pos> cube_rock_lookup;
    int width;
    int height;

  private:
    bool is_in_bounds(const Pos &pos) {
        return pos.x >= 0 && pos.y >= 0 && pos.x < width && pos.y < height;
    }

    bool move_rock(std::size_t index, const Delta &delta) {
        Pos &pos = round_rocks[index];
        Pos new_pos = pos + delta;
        if (!is_in_bounds(new_pos)) {
            return false;
        }
        if (cube_rock_lookup.contains(new_pos) ||
            round_rock_lookup.contains(new_pos)) {
            return false;
        }
        round_rock_lookup.erase(pos);
        pos = new_pos;
        round_rock_lookup.insert(pos);
        return true;
    }

  public:
    void add_rock(const Pos &pos, bool round);
    void tilt(Direction dir);
    int calculate_load() const;
};

void Platform::add_rock(const Pos &pos, bool round) {
    if (round) {
        round_rock_lookup.insert(pos);
        round_rocks.push_back(pos);
    } else {
        cube_rock_lookup.insert(pos);
    }
}

void Platform::tilt(Direction dir) {
    Delta delta{dir, true};
    // most naive method: bubble sort
    bool moved;
    do {
        moved = false;
        for (std::size_t i = 0; i < round_rocks.size(); ++i) {
            moved |= move_rock(i, delta);
        }
    } while (moved);
}

int Platform::calculate_load() const {
    int load = 0;
    for (const Pos &pos : round_rocks) {
        load += height - pos.y;
    }
    return load;
}

std::ostream &operator<<(std::ostream &os, const Platform &platform) {
    Pos pos;
    for (pos.y = 0; pos.y < platform.height; ++pos.y) {
        for (pos.x = 0; pos.x < platform.width; ++pos.x) {
            char ch = '.';
            if (platform.round_rock_lookup.contains(pos)) {
                ch = 'O';
            } else if (platform.cube_rock_lookup.contains(pos)) {
                ch = '#';
            }
            os << ch;
        }
        os << '\n';
    }

    return os;
}

Platform read_platform(std::istream &is) {
    Platform platform{};
    std::string line;
    Pos pos;
    for (pos.y = 0; std::getline(is, line); ++pos.y) {
        for (pos.x = 0; pos.x < static_cast<int>(line.size()); ++pos.x) {
            char ch = line[pos.x];
            if (ch == '.') {
                continue;
            }
            platform.add_rock(pos, ch == 'O');
        }
    }
    platform.width = pos.x;
    platform.height = pos.y;
    return platform;
}

} // namespace aoc::day14
