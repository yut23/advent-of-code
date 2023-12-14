/******************************************************************************
 * File:        day14.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-14
 *****************************************************************************/

#include "lib.hpp"  // for Pos, Delta
#include <cstddef>  // for size_t
#include <iostream> // for istream
#include <map>      // for map
#include <string>   // for string, getline
#include <vector>   // for vector

namespace aoc::day14 {

struct Rock {
    Pos pos;
    bool round;

    Rock(const Pos &pos, bool round) : pos(pos), round(round) {}
};

struct Platform {
    std::vector<Rock> rocks;
    std::map<Pos, std::size_t> rock_lookup;
    int width;
    int height;

  private:
    bool is_in_bounds(const Pos &pos) {
        return pos.x >= 0 && pos.y >= 0 && pos.x < width && pos.y < height;
    }

    bool move_rock(std::size_t index, const Delta &delta) {
        Rock &rock = rocks[index];
        Pos new_pos = rock.pos + delta;
        if (!is_in_bounds(new_pos)) {
            return false;
        }
        if (rock_lookup.contains(new_pos)) {
            return false;
        }
        rock_lookup.erase(rock.pos);
        rock.pos = new_pos;
        rock_lookup[rock.pos] = index;
        return true;
    }

  public:
    void add_rock(const Pos &pos, bool round);
    void tilt(Direction dir);
    int calculate_load() const;
};

void Platform::add_rock(const Pos &pos, bool round) {
    rock_lookup.try_emplace(pos, rocks.size());
    rocks.emplace_back(pos, round);
}

void Platform::tilt(Direction dir) {
    Delta delta{dir, true};
    // most naive method: bubble sort
    bool moved;
    do {
        moved = false;
        for (std::size_t i = 0; i < rocks.size(); ++i) {
            if (rocks[i].round) {
                moved |= move_rock(i, delta);
            }
        }
    } while (moved);
}

int Platform::calculate_load() const {
    int load = 0;
    for (const Rock &rock : rocks) {
        if (rock.round) {
            load += height - rock.pos.y;
        }
    }
    return load;
}

std::ostream &operator<<(std::ostream &os, const Platform &platform) {
    Pos pos;
    for (pos.y = 0; pos.y < platform.height; ++pos.y) {
        for (pos.x = 0; pos.x < platform.width; ++pos.x) {
            auto it = platform.rock_lookup.find(pos);
            if (it == platform.rock_lookup.end()) {
                os << '.';
            } else {
                os << (platform.rocks[it->second].round ? 'O' : '#');
            }
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
