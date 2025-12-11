/******************************************************************************
 * File:        day10.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-12-10
 *****************************************************************************/

#ifndef DAY10_HPP_STNSRZ3A
#define DAY10_HPP_STNSRZ3A

#include "lib.hpp"  // for read_vector
#include <iostream> // for istream
#include <string>   // for basic_string
#include <utility>  // for move
#include <vector>   // for vector

namespace aoc::day10 {

struct Button {
    std::vector<int> light_indices;

    friend std::istream &operator>>(std::istream &is, Button &dest);
    friend std::ostream &operator<<(std::ostream &os, const Button &button);
};

struct Machine {
    std::vector<bool> target_lights;
    std::vector<Button> buttons;
    std::vector<int> joltage_requirements;

    friend std::istream &operator>>(std::istream &is, Machine &dest);
    friend std::ostream &operator>>(std::ostream &os, const Machine &machine);
};

struct BfsEntry {
    const Machine &machine;
    std::vector<bool> pressed_buttons{};
    std::vector<bool> lights{};

    explicit BfsEntry(const Machine &machine)
        : machine(machine), pressed_buttons(machine.buttons.size(), false),
          lights(machine.target_lights.size(), false) {}
    BfsEntry(const BfsEntry &) = default;
    BfsEntry &operator=(const BfsEntry &) = delete;
    BfsEntry(BfsEntry &&) noexcept = default;
    BfsEntry &operator=(BfsEntry &&) noexcept = delete;

    bool operator==(const BfsEntry &other) const {
        return &machine == &other.machine &&
               pressed_buttons == other.pressed_buttons &&
               lights == other.lights;
    }
    // std::strong_ordering operator<=>(const BfsEntry &other) const {
    //     if (auto cmp = &machine <=> &other.machine; cmp != 0)
    //         return cmp;
    //     if (auto cmp = pressed_buttons <=> other.pressed_buttons; cmp != 0)
    //         return cmp;
    //     return lights <=> other.lights;
    // }

    BfsEntry press_button(std::size_t i) const {
        assert(!pressed_buttons[i]);
        BfsEntry new_entry = *this;
        new_entry.pressed_buttons[i] = true;
        for (auto light_idx : machine.buttons[i].light_indices) {
            new_entry.lights[light_idx].flip();
        }
        return new_entry;
    }

    static bool is_target(const BfsEntry &entry) {
        return entry.lights == entry.machine.target_lights;
    }

    static void process_neighbors(const BfsEntry &entry, const auto &process) {
        for (std::size_t i = 0; i < entry.pressed_buttons.size(); ++i) {
            if (entry.pressed_buttons[i]) {
                continue;
            }
            process(entry.press_button(i));
        }
    }
};
} // namespace aoc::day10

template <>
struct std::hash<aoc::day10::BfsEntry> {
    std::size_t operator()(const aoc::day10::BfsEntry &entry) const noexcept {
        // random number (hexdump -n8 -e '"0x" 8/1 "%02x" "ull\n"'</dev/urandom)
        std::size_t seed = 0x302e3ffbabe1fca6ull;
        util::make_hash(seed, entry.lights, entry.pressed_buttons);
        return seed;
    }
};

namespace aoc::day10 {

// I/O operations
std::istream &operator>>(std::istream &is, Button &dest) {
    Button button{};
    is >> std::ws;
    is >> aoc::expect_input('(') >> aoc::csv_formatter(button.light_indices) >>
        aoc::expect_input(')');
    if (is) {
        dest = std::move(button);
    }
    return is;
}

std::ostream &operator<<(std::ostream &os, const Button &button) {
    os << '(' << aoc::csv_formatter(button.light_indices) << ')';
    return os;
}

std::istream &operator>>(std::istream &is, Machine &dest) {
    Machine machine{};
    is >> std::ws;
    // read the indicator lights
    is >> aoc::expect_input('[');
    char c;
    while (is >> c) {
        if (c == '.') {
            machine.target_lights.push_back(false);
        } else if (c == '#') {
            machine.target_lights.push_back(true);
        } else {
            break;
        }
    }
    if (c != ']') {
        is.setstate(std::ios_base::failbit);
        return is;
    }

    // read the buttons
    Button button;
    while (is >> std::ws && is.peek() == '(') {
        is >> button;
        machine.buttons.push_back(std::move(button));
    }

    // read the joltage requirements
    is >> aoc::expect_input('{') >>
        aoc::csv_formatter(machine.joltage_requirements) >>
        aoc::expect_input('}');

    assert(machine.joltage_requirements.size() == machine.target_lights.size());

    if (is) {
        dest = std::move(machine);
    }
    return is;
}

std::ostream &operator<<(std::ostream &os, const Machine &machine) {
    os << '[';
    for (bool is_on : machine.target_lights) {
        os << (is_on ? '#' : '.');
    }
    os << "] ";
    for (const Button &button : machine.buttons) {
        os << button << ' ';
    }
    os << '{' << aoc::csv_formatter(machine.joltage_requirements) << '}';
    return os;
}

auto read_input(std::istream &is) { return aoc::read_vector<Machine>(is); }

} // namespace aoc::day10

#endif /* end of include guard: DAY10_HPP_STNSRZ3A */
