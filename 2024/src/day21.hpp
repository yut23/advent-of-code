/******************************************************************************
 * File:        day21.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-01-27
 *****************************************************************************/

#ifndef DAY21_HPP_1U0ZBGSD
#define DAY21_HPP_1U0ZBGSD

#include "lib.hpp"                    // for Pos, Delta, DEBUG
#include "unit_test/pretty_print.hpp" // for repr

#include <algorithm>   // for ranges::transform
#include <cassert>     // for assert
#include <compare>     // for strong_ordering
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

enum class Key : char {
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
    UP,
    DOWN,
    LEFT,
    RIGHT,
};

std::istream &operator>>(std::istream &is, Key &key) {
    char ch;
    is >> ch;
    if (ch >= '0' && ch <= '9') {
        key = Key(ch - '0');
    } else {
        switch (ch) {
            using enum Key;
        case '^':
            key = UP;
            break;
        case 'v':
        case 'V':
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
    }
    return is;
}

std::ostream &operator<<(std::ostream &os, const Key &key) {
    switch (key) {
        using enum Key;
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
    default:
        os << aoc::as_number(static_cast<std::underlying_type_t<Key>>(key));
        break;
    }
    return os;
}

// overload print_repr for a vector of Keys
// This will be picked up by ADL, and is more specialized than the generic
// iterable collection version in pretty_print.hpp, so it will be picked.
std::ostream &print_repr(std::ostream &os, const std::vector<Key> &keys,
                         const pretty_print::repr_state /*state*/) {
    for (const auto key : keys) {
        os << key;
    }
    return os;
}

// these positions are set up so the empty space is at (0, 0)
const std::map<Key, Pos> KEY_POSITIONS = {
    // clang-format off
    {Key(7),    {0,  3}}, {Key(8),    {1,  3}}, {Key(9),        {2,  3}},
    {Key(4),    {0,  2}}, {Key(5),    {1,  2}}, {Key(6),        {2,  2}},
    {Key(1),    {0,  1}}, {Key(2),    {1,  1}}, {Key(3),        {2,  1}},
                          {Key(0),    {1,  0}}, {Key::ACTIVATE, {2,  0}},
                          {Key::UP,   {1,  0}},
    {Key::LEFT, {0, -1}}, {Key::DOWN, {1, -1}}, {Key::RIGHT,    {2, -1}},
    // clang-format on
};

/**
 * Appends the sequence of directional keys to `outputs` that will move a
 * robotic arm between `from` and `to` on a keypad.
 */
void move_arm(const Pos &from, const Pos &to, std::vector<Key> &outputs) {
    Delta delta = to - from;
    using enum Key;
    const auto move = [&outputs, &delta](Key key) {
        int count = 0;
        switch (key) {
        case RIGHT:
            count = delta.dx;
            break;
        case LEFT:
            count = -delta.dx;
            break;
        case UP:
            count = delta.dy;
            break;
        case DOWN:
            count = -delta.dy;
            break;
        default:
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

bool check_keys(const std::vector<Key> &keys,
                const std::vector<Pos> &key_positions, const Pos &lower_left,
                const Pos &upper_right) {
    Pos curr_pos = KEY_POSITIONS.at(Key::ACTIVATE);
    auto it = key_positions.begin();
    for (std::size_t i = 0; i < keys.size(); ++i) {
        switch (keys[i]) {
        case Key::RIGHT:
            ++curr_pos.x;
            break;
        case Key::LEFT:
            --curr_pos.x;
            break;
        case Key::UP:
            ++curr_pos.y;
            break;
        case Key::DOWN:
            --curr_pos.y;
            break;
        case Key::ACTIVATE:
            if (it == key_positions.end()) {
                std::cerr << "Error: pressed activate too many times in "
                          << pretty_print::repr(keys) << "\n";
                return false;
            }
            if (curr_pos != *it) {
                std::cerr
                    << "Error: pressed activate with arm at wrong position in "
                    << pretty_print::repr(keys) << ": expected " << *it
                    << ", got " << curr_pos << "\n";
                return false;
            }
            ++it;
            break;
        default:
            break;
        }
        if (curr_pos.x == 0 && curr_pos.y == 0) {
            std::cerr << "Error: moved through empty space in "
                      << pretty_print::repr(keys) << "\n";
            return false;
        }
        if (curr_pos.x < lower_left.x || curr_pos.y < lower_left.y ||
            curr_pos.x > upper_right.x || curr_pos.y > upper_right.y) {
            std::cerr << "Error: moved off of keypad in "
                      << pretty_print::repr(keys) << "\n";
            return false;
        }
    }
    if (it != key_positions.end()) {
        std::cerr << "Error: didn't press activate enough times in "
                  << pretty_print::repr(keys) << ": expected "
                  << key_positions.size() << "\n";
        return false;
    }
    return true;
}

/**
 * Returns a sequence of keys to press on a directional keypad for a robotic
 * arm to press the given sequence on another keypad.
 */
std::vector<Key> control_arm(const std::vector<Key> &inputs) {
    std::vector<Key> outputs;
    Pos curr_pos = KEY_POSITIONS.at(Key::ACTIVATE);
    for (const auto key : inputs) {
        Pos new_pos = KEY_POSITIONS.at(key);
        move_arm(curr_pos, new_pos, outputs);
        outputs.push_back(Key::ACTIVATE);
        curr_pos = new_pos;
    }
    if constexpr (aoc::DEBUG) {
        std::vector<Pos> input_positions(inputs.size());
        std::ranges::transform(inputs, input_positions.begin(),
                               [](auto key) { return KEY_POSITIONS.at(key); });
        bool valid =
            check_keys(outputs, input_positions, KEY_POSITIONS.at(Key::LEFT),
                       KEY_POSITIONS.at(Key::NINE));
        assert(valid);
    }
    return outputs;
}

struct RobotController {
    int layers;
    std::vector<Key> last_key;

    struct CacheKey {
        int layer;
        Key key;
        Key prev_key;

        std::strong_ordering operator<=>(const CacheKey &) const = default;
    };
    std::map<CacheKey, long> count_cache{};

    explicit RobotController(int layers_)
        : layers(layers_), last_key(layers + 1, Key::ACTIVATE) {}

    long press_key(Key key, int depth = 0);
    long press_key_memo(Key key, int depth = 0);

    long count_presses(const std::vector<Key> &keys, int layers);

  protected:
    virtual void on_keypress([[maybe_unused]] Key key,
                             [[maybe_unused]] int layer) {}
};

long RobotController::press_key(Key key, int depth) {
    // layer: how many robots away from the human we are (0 for the human)
    int layer = layers - depth;
    if constexpr (aoc::DEBUG) {
        std::cerr << std::string(depth, ' ') << "layer " << layer
                  << ": pressing " << key;
    }

    long count = 0;
    if (layer == 0) {
        if constexpr (aoc::DEBUG) {
            std::cerr << " (human)\n";
        }
        count = 1;
    } else {
        std::vector<Key> key_sequence;
        move_arm(KEY_POSITIONS.at(last_key[layer]), KEY_POSITIONS.at(key),
                 key_sequence);
        key_sequence.push_back(Key::ACTIVATE);

        if constexpr (aoc::DEBUG) {
            std::cerr << ": " << last_key[layer - 1] << "|"
                      << pretty_print::repr(key_sequence) << "\n";
        }

        for (const auto k : key_sequence) {
            count += press_key(k, depth + 1);
        }
    }
    last_key[layer] = key;
    on_keypress(key, layer);
    return count;
}

long RobotController::press_key_memo(Key key, int depth) {
    // layer: how many robots away from the human we are (0 for the human)
    int layer = layers - depth;
    if constexpr (aoc::DEBUG) {
        std::cerr << std::string(depth, ' ') << "layer " << layer
                  << ": pressing " << key;
    }

    long count = 0;
    if (layer == 0) {
        if constexpr (aoc::DEBUG) {
            std::cerr << " (human)\n";
        }
        count = 1;
    } else {
        CacheKey cache_key{layer, key, last_key[layer]};
        auto it = count_cache.find(cache_key);
        if (it != count_cache.end()) {
            count = it->second;
        } else {
            std::vector<Key> key_sequence;
            move_arm(KEY_POSITIONS.at(last_key[layer]), KEY_POSITIONS.at(key),
                     key_sequence);
            key_sequence.push_back(Key::ACTIVATE);

            if constexpr (aoc::DEBUG) {
                std::cerr << ": " << last_key[layer - 1] << "|"
                          << pretty_print::repr(key_sequence) << "\n";
            }

            for (const auto k : key_sequence) {
                count += press_key_memo(k, depth + 1);
            }
            count_cache.emplace_hint(it, cache_key, count);
        }
    }
    last_key[layer] = key;
    return count;
}

long count_presses_bfs(std::vector<Key> keys, int layers) {
    for (int i = 0; i < layers; ++i) {
        keys = control_arm(keys);
    }
    return keys.size();
}

long count_presses_dfs(const std::vector<Key> &keys, int layers) {
    RobotController controller(layers);
    long count = 0;
    for (Key key : keys) {
        count += controller.press_key(key);
    }
    return count;
}

long count_presses_memo(const std::vector<Key> &keys, int layers) {
    RobotController controller(layers);
    long count = 0;
    for (Key key : keys) {
        count += controller.press_key_memo(key);
    }
    return count;
}

long RobotController::count_presses(const std::vector<Key> &keys, int layers) {
    long count = 0;
    for (Key key : keys) {
        count += press_key_memo(key, this->layers - layers);
    }
    return count;
}

struct Code {
    std::string code;
    std::vector<Key> keys{};

    int numeric_value() const { return std::stoi(code); }
};

std::vector<Code> read_input(std::istream &is) {
    std::vector<Code> codes;
    std::string line;
    while (std::getline(is, line)) {
        std::istringstream iss{line};
        codes.push_back({std::move(line), aoc::read_vector<Key>(iss)});
    }
    return codes;
}

} // namespace aoc::day21

#endif /* end of include guard: DAY21_HPP_1U0ZBGSD */
