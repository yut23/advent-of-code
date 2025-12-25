/******************************************************************************
 * File:        day10.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-12-10
 *****************************************************************************/

#ifndef DAY10_HPP_STNSRZ3A
#define DAY10_HPP_STNSRZ3A

#include "ds/grid.hpp"         // for Grid
#include "gauss_elim.hpp"      // for RowPermuter, gauss_jordan
#include "graph_traversal.hpp" // for bfs
#include "lib.hpp"             // for read_vector, expect_input, csv_formatter
#ifdef DEBUG_MODE
#include "unit_test/pretty_print.hpp"
#endif

#include <algorithm>  // for ranges::copy, ranges::max
#include <cassert>    // for assert
#include <compare>    // for strong_ordering
#include <cstddef>    // for size_t
#include <cstdint>    // for uint16_t
#include <functional> // for identity, less (ranges::max)
#include <iomanip>    // for setw
#include <iostream>   // for istream, ostream, ios_base, ws
#include <numeric>    // for reduce
#include <optional>   // for optional
#include <utility>    // for move
#include <vector>     // for vector

namespace aoc::day10 {

struct Button {
    std::vector<int> light_indices;
};
std::istream &operator>>(std::istream &is, Button &dest);
std::ostream &operator<<(std::ostream &os, const Button &button);

struct Machine {
    std::vector<bool> target_lights;
    std::vector<Button> buttons;
    std::vector<uint16_t> joltage_requirements;

    template <aoc::Part PART>
    int min_presses() const;
};
std::istream &operator>>(std::istream &is, Machine &dest);
std::ostream &operator<<(std::ostream &os, const Machine &machine);

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
               pressed_buttons == other.pressed_buttons;
    }
    std::strong_ordering operator<=>(const BfsEntry &other) const {
        if (auto cmp = &machine <=> &other.machine; cmp != 0)
            return cmp;
        return pressed_buttons <=> other.pressed_buttons;
    }

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

template <>
int Machine::min_presses<aoc::PART_1>() const {
    // generalized BFS!
    const auto process_neighbors = [](const BfsEntry &entry,
                                      const auto &process) {
        BfsEntry::process_neighbors(entry, process);
    };
    return aoc::graph::bfs(BfsEntry(*this), process_neighbors,
                           BfsEntry::is_target, {});
}

struct Part2Solver {
    const Machine &machine;
    aoc::ds::Grid<int> mtx;
    std::vector<int> rhs;
    aoc::math::RowPermuter rp;

    explicit Part2Solver(const Machine &machine_);

    void reduce_matrix();
    int min_presses();

  private:
    std::vector<int> free_vars{};
    int upper_bound;

    std::optional<int>
    total_presses(const std::vector<int> &free_presses) const;

    // returns the minimum number of presses, or {} if no valid assignments
    // were found
    std::optional<int> recurse_free_presses(std::vector<int> &presses) const;
};

Part2Solver::Part2Solver(const Machine &machine_)
    : machine(machine_),
      mtx(machine.buttons.size(), machine.joltage_requirements.size()),
      rhs(machine.joltage_requirements.size()), rp(mtx.height) {
    // fill the matrix and right-hand side
    for (std::size_t x = 0; x < machine.buttons.size(); ++x) {
        for (int y : machine.buttons[x].light_indices) {
            mtx.at(x, y) = 1;
        }
    }
    std::ranges::copy(machine.joltage_requirements, rhs.begin());
    upper_bound = std::ranges::max(machine.joltage_requirements);
}

template <>
int Machine::min_presses<aoc::PART_2>() const {
    const Machine &machine = *this;
    if constexpr (aoc::DEBUG) {
        std::cerr << machine << "\n";
    }
    auto result = Part2Solver(machine).min_presses();
    if constexpr (aoc::DEBUG) {
        std::cerr << "result: " << result << " presses\n";
    }
    return result;
}

void Part2Solver::reduce_matrix() {
    if constexpr (aoc::DEBUG) {
        std::cerr << "before:\n" << std::setw(2);
        rp.pretty_print(std::cerr, mtx, rhs);
    }
    {
        auto tmp = aoc::math::gauss_jordan(mtx, rhs);
        if (tmp.has_value()) {
            rp = std::move(tmp).value();
        }
    }
    if constexpr (aoc::DEBUG) {
        std::cerr << "after:\n" << std::setw(2);
        rp.pretty_print(std::cerr, mtx, rhs);
    }

    for (int c = 0; c < mtx.width; ++c) {
        int nonzero_count = 0;
        for (int r = 0; r < mtx.height && nonzero_count <= 1; ++r) {
            if (rp(mtx, r, c) != 0) {
                ++nonzero_count;
            }
        }
        if (nonzero_count != 1) {
            free_vars.push_back(c);
        }
    }
}

int Part2Solver::min_presses() {
    reduce_matrix();

    if constexpr (aoc::DEBUG) {
        if (free_vars.empty()) {
            std::cerr << "exactly determined\n";
        } else {
            std::cerr << "underdetermined, " << free_vars.size()
                      << " free variable" << (free_vars.size() == 1 ? "" : "s")
                      << "\n";
        }
    }

    if (free_vars.empty()) {
        // number of presses == sum(rhs)
        return std::reduce(rhs.begin(), rhs.end());
    } else {
        std::vector<int> presses;
        auto count = recurse_free_presses(presses);
        assert(count.has_value());
        return *count;
    }
}

std::optional<int>
Part2Solver::total_presses(const std::vector<int> &free_presses) const {
    auto free_vars_it = free_vars.begin();
    auto free_presses_it = free_presses.begin();
    int r = 0;
    std::vector<int> presses(mtx.width);
    for (int c = 0; c < mtx.width; ++c) {
        if (free_vars_it != free_vars.end() && c == *free_vars_it) {
            // free variable, substitute the value from the arguments
            presses[c] = *free_presses_it++;
            ++free_vars_it;
        } else {
            // constrained variable, calculate from matrix
            //   mtx . [presses[c], *free_presses] = rhs[r]
            //   presses[c] = (rhs[r] - sum_i(mtx[r, free_vars[i]]
            //                                * free_presses[i])) / mtx[r, c]
            int denom = rp(mtx, r, c);
            if (denom != 0) {
                int tmp = rp(rhs, r);
                for (std::size_t i = 0; i < free_presses.size(); ++i) {
                    tmp -= rp(mtx, r, free_vars[i]) * free_presses[i];
                }
                if (tmp % denom != 0) {
                    return {};
                }
                presses[c] = tmp / denom;
                ++r;
            }
        }
        if (presses[c] < 0) {
            return {};
        }
    }
    return std::reduce(presses.begin(), presses.end());
}

std::optional<int>
Part2Solver::recurse_free_presses(std::vector<int> &presses) const {
    if (presses.size() == free_vars.size()) {
        auto result = total_presses(presses);
        return result;
    }
    // add a new free variable to the end
    std::size_t i = presses.size();
    presses.push_back(0);
    std::optional<int> best = {};
    for (int x = 0; x < upper_bound; ++x) {
        presses[i] = x;
        auto result = recurse_free_presses(presses);
        if (result.has_value() && (!best.has_value() || *result < *best)) {
            best = std::move(result);
        }
    }
    presses.pop_back();
    return best;
}

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
