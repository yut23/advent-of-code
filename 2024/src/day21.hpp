/******************************************************************************
 * File:        day21.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-01-27
 *****************************************************************************/

#ifndef DAY21_HPP_1U0ZBGSD
#define DAY21_HPP_1U0ZBGSD

#include "lib.hpp"     // for Pos, Delta, DEBUG
#include <algorithm>   // for ranges::transform
#include <cassert>     // for assert
#include <cstddef>     // for size_t
#include <functional>  // for identity (transform)
#include <iostream>    // for istream, ostream, cerr,
#include <map>         // for map
#include <sstream>     // for istringstream
#include <string>      // for string, getline, stoi
#include <type_traits> // for underlying_type_t
#include <utility>     // for move
#include <vector>      // for vector

namespace aoc::day21 {

enum class NumKey {
    ZERO = 0,
    ONE = 1,
    TWO = 2,
    THREE = 3,
    FOUR = 4,
    FIVE = 5,
    SIX = 6,
    SEVEN = 7,
    EIGHT = 8,
    NINE = 9,
    ACTIVATE,
};

std::istream &operator>>(std::istream &is, NumKey &key) {
    char ch;
    is >> ch;
    if (ch == 'A') {
        key = NumKey::ACTIVATE;
    } else if (ch >= '0' && ch <= '9') {
        key = NumKey(ch - '0');
    } else {
        is.setstate(std::ios_base::failbit);
    }
    return is;
}

std::ostream &operator<<(std::ostream &os, const NumKey &key) {
    if (key == NumKey::ACTIVATE) {
        os << 'A';
    } else {
        os << '0' + static_cast<std::underlying_type_t<NumKey>>(key);
    }
    return os;
}

enum class DirKey {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    ACTIVATE,
};

std::istream &operator>>(std::istream &is, DirKey &key) {
    char ch;
    is >> ch;
    switch (ch) {
        using enum DirKey;
    case '^':
        key = UP;
        break;
    case 'v':
        key = DOWN;
        break;
    case '<':
        key = LEFT;
        break;
    case '>':
        key = RIGHT;
        break;
    case 'A':
        key = ACTIVATE;
        break;
    default:
        is.setstate(std::ios_base::failbit);
        break;
    }
    return is;
}

std::ostream &operator<<(std::ostream &os, const DirKey &key) {
    switch (key) {
        using enum DirKey;
    case UP:
        os << '^';
        break;
    case DOWN:
        os << 'v';
        break;
    case LEFT:
        os << '<';
        break;
    case RIGHT:
        os << '>';
        break;
    case ACTIVATE:
        os << 'A';
        break;
    }
    return os;
}

// these positions are set up so the empty space is at (0, 0)
const std::map<NumKey, Pos> NUM_POSITIONS = {
    // clang-format off
    {NumKey(7), {0, 3}}, {NumKey(8), {1, 3}}, {NumKey(9),        {2, 3}},
    {NumKey(4), {0, 2}}, {NumKey(5), {1, 2}}, {NumKey(6),        {2, 2}},
    {NumKey(1), {0, 1}}, {NumKey(2), {1, 1}}, {NumKey(3),        {2, 1}},
                         {NumKey(0), {1, 0}}, {NumKey::ACTIVATE, {2, 0}},
    // clang-format on
};

const std::map<DirKey, Pos> DIR_POSITIONS = {
    // clang-format off
                             {DirKey::UP,   {1,  0}}, {DirKey::ACTIVATE, {2,  0}},
    {DirKey::LEFT, {0, -1}}, {DirKey::DOWN, {1, -1}}, {DirKey::RIGHT,    {2, -1}},
    // clang-format on
};

/**
 * Appends the sequence of directional keys to `outputs` that will move a
 * robotic arm between `from` and `to` on a keypad.
 */
void move_arm(const Pos &from, const Pos &to, std::vector<DirKey> &outputs) {
    Delta delta = to - from;
    using enum DirKey;
    const auto move = [&outputs, &delta](DirKey key) {
        int count = 0;
        switch (key) {
        case DirKey::RIGHT:
            count = delta.dx;
            break;
        case DirKey::LEFT:
            count = -delta.dx;
            break;
        case DirKey::UP:
            count = delta.dy;
            break;
        case DirKey::DOWN:
            count = -delta.dy;
            break;
        case DirKey::ACTIVATE:
            break;
        }
        for (; count > 0; --count) {
            outputs.push_back(key);
        }
    };

    // check if we would move through the empty space at (0, 0)
    if ((from.x == 0 && to.y == 0) || (from.y == 0 && to.x == 0)) {
        // always move right before moving up or down, and up or down before
        // moving left to avoid the empty space
        move(RIGHT);
        move(UP);
        move(DOWN);
        move(LEFT);
    } else {
        // order movements to minimize parent arm travel
        move(LEFT);
        move(UP);
        move(DOWN);
        move(RIGHT);
    }
}

bool check_keys(const std::vector<DirKey> &keys,
                const std::vector<Pos> &key_positions, const Pos &lower_left,
                const Pos &upper_right) {
    std::ostringstream formatted_keys;
    Pos curr_pos = DIR_POSITIONS.at(DirKey::ACTIVATE);
    auto it = key_positions.begin();
    for (std::size_t i = 0; i < keys.size(); ++i) {
        formatted_keys << keys[i];
        switch (keys[i]) {
        case DirKey::RIGHT:
            ++curr_pos.x;
            break;
        case DirKey::LEFT:
            --curr_pos.x;
            break;
        case DirKey::UP:
            ++curr_pos.y;
            break;
        case DirKey::DOWN:
            --curr_pos.y;
            break;
        case DirKey::ACTIVATE:
            if (it == key_positions.end()) {
                std::cerr << "Error: pressed activate too many times in "
                          << formatted_keys.str() << "\n";
                return false;
            }
            if (curr_pos != *it) {
                std::cerr
                    << "Error: pressed activate with arm at wrong position in "
                    << formatted_keys.str() << ": expected " << *it << ", got "
                    << curr_pos << "\n";
                return false;
            }
            ++it;
            break;
        }
        if (curr_pos.x == 0 && curr_pos.y == 0) {
            std::cerr << "Error: moved through empty space in "
                      << formatted_keys.str() << "\n";
            return false;
        }
        if (curr_pos.x < lower_left.x || curr_pos.y < lower_left.y ||
            curr_pos.x > upper_right.x || curr_pos.y > upper_right.y) {
            std::cerr << "Error: moved off of keypad in "
                      << formatted_keys.str() << "\n";
            return false;
        }
    }
    if (it != key_positions.end()) {
        std::cerr << "Error: didn't press activate enough times in "
                  << formatted_keys.str() << ": expected "
                  << key_positions.size() << "\n";
        return false;
    }
    return true;
}

/**
 * Returns a sequence of keys to press on a directional keypad for a robotic
 * arm to press the given sequence on a numeric keypad.
 */
std::vector<DirKey> control_numeric(const std::vector<NumKey> &keys) {
    std::vector<DirKey> outputs;
    Pos curr_pos = NUM_POSITIONS.at(NumKey::ACTIVATE);
    for (const auto key : keys) {
        Pos new_pos = NUM_POSITIONS.at(key);
        move_arm(curr_pos, new_pos, outputs);
        outputs.push_back(DirKey::ACTIVATE);
        curr_pos = new_pos;
    }
    if constexpr (aoc::DEBUG) {
        std::vector<Pos> key_positions(keys.size());
        std::ranges::transform(keys, key_positions.begin(),
                               [](auto key) { return NUM_POSITIONS.at(key); });
        bool valid = check_keys(outputs, key_positions, {0, 0},
                                NUM_POSITIONS.at(NumKey::NINE));
        assert(valid);
    }
    return outputs;
}

/**
 * Returns a sequence of keys to press on a directional keypad for a robotic
 * arm to press the given sequence on another directional keypad.
 */
std::vector<DirKey> control_directional(const std::vector<DirKey> &keys) {
    std::vector<DirKey> outputs;
    Pos curr_pos = DIR_POSITIONS.at(DirKey::ACTIVATE);
    for (const auto key : keys) {
        Pos new_pos = DIR_POSITIONS.at(key);
        move_arm(curr_pos, new_pos, outputs);
        outputs.push_back(DirKey::ACTIVATE);
        curr_pos = new_pos;
    }
    if constexpr (aoc::DEBUG) {
        std::vector<Pos> key_positions(keys.size());
        std::ranges::transform(keys, key_positions.begin(),
                               [](auto key) { return DIR_POSITIONS.at(key); });
        bool valid =
            check_keys(outputs, key_positions, DIR_POSITIONS.at(DirKey::LEFT),
                       DIR_POSITIONS.at(DirKey::ACTIVATE));
        assert(valid);
    }
    return outputs;
}

struct Code {
    std::string code;
    std::vector<NumKey> keys{};

    int numeric_value() const { return std::stoi(code); }
};

std::vector<Code> read_input(std::istream &is) {
    std::vector<Code> codes;
    std::string line;
    while (std::getline(is, line)) {
        std::istringstream iss{line};
        Code code{std::move(line)};
        NumKey key{};
        while (iss >> key) {
            code.keys.push_back(key);
        }
        codes.push_back(std::move(code));
    }
    return codes;
}

} // namespace aoc::day21

#endif /* end of include guard: DAY21_HPP_1U0ZBGSD */
