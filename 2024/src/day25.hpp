/******************************************************************************
 * File:        day25.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-01-27
 *****************************************************************************/

#ifndef DAY25_HPP_DOJZBHTX
#define DAY25_HPP_DOJZBHTX

#include "unit_test/pretty_print.hpp" // for repr

#include <array>    // for array
#include <iostream> // for istream
#include <string>   // for string, getline
#include <utility>  // for pair
#include <vector>   // for vector

namespace aoc::day25 {

struct Schematic {
    bool is_key;
    std::array<char, 5> pins;

    bool check_fit(const Schematic &other) const {
        if (this->is_key == other.is_key) {
            return false;
        }
        if (this->is_key) {
            return other.check_fit(*this);
        }
        // *this is a lock, other is a key
        for (unsigned int i = 0; i < pins.size(); ++i) {
            if (pins[i] + other.pins[i] > 5) {
                return false;
            }
        }
        return true;
    }
};

std::istream &operator>>(std::istream &is, Schematic &schematic) {
    Schematic tmp{};
    std::string line;
    // consume blank line if present
    is >> std::ws;
    if (!std::getline(is, line)) {
        return is;
    }
    tmp.is_key = line[0] == '.';
    for (int i = 0; i < 5; ++i) {
        if (!std::getline(is, line)) {
            break;
        }
        for (int j = 0; j < 5; ++j) {
            tmp.pins[j] += line[j] == '#';
        }
    }
    std::getline(is, line);
    if (is) {
        std::swap(schematic, tmp);
    }
    return is;
}

std::ostream &operator<<(std::ostream &os, const Schematic &schematic) {
    os << (schematic.is_key ? "key" : "lock") << " ";
    os << pretty_print::repr(schematic.pins, {.char_as_number = true});
    return os;
}

std::pair<std::vector<Schematic>, std::vector<Schematic>>
read_input(std::istream &is) {
    std::vector<Schematic> keys;
    std::vector<Schematic> locks;
    Schematic schematic;
    while (is >> schematic) {
        if (schematic.is_key) {
            keys.push_back(std::move(schematic));
        } else {
            locks.push_back(std::move(schematic));
        }
    }
    return {keys, locks};
}

} // namespace aoc::day25

#endif /* end of include guard: DAY25_HPP_DOJZBHTX */
