/******************************************************************************
 * File:        day15.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-15
 *****************************************************************************/

#ifndef DAY_HPP_SD0ITZCR
#define DAY_HPP_SD0ITZCR

#include <algorithm> // for find_if
#include <array>     // for array
#include <cstddef>   // for size_t
#include <cstdint>   // for uint8_t
#include <iostream>  // for istream
#include <list>      // for list
#include <string>    // for string, getline
#include <utility>   // for move, swap
#include <vector>    // for vector

namespace aoc::day15 {

std::uint8_t hash_algorithm(const std::string &s) {
    int value = 0;
    for (char c : s) {
        value += c;
        value *= 17;
        value %= 256;
    }
    return value;
}

std::vector<std::string> read_steps(std::istream &is) {
    std::vector<std::string> steps;
    std::string tmp;
    while (std::getline(is, tmp, ',')) {
        // remove trailing newline(s)
        while (tmp.ends_with('\n')) {
            tmp.pop_back();
        }
        steps.push_back(tmp);
    }
    return steps;
}

struct Lens {
    std::string label;
    int focal_length;
};

class Hashmap {
    std::array<std::list<Lens>, 256> boxes;

  public:
    void operate(const std::string &step);
    int focusing_power() const;
};

void Hashmap::operate(const std::string &step) {
    auto it = std::find_if(step.begin(), step.end(),
                           [](char ch) { return ch == '=' || ch == '-'; });
    std::string label(step.begin(), it);
    std::list<Lens> &box = boxes[hash_algorithm(label)];
    auto lens_it =
        std::find_if(box.begin(), box.end(), [&label](const Lens &lens) {
            return lens.label == label;
        });
    if (*it == '=') {
        // insert a lens
        int focal_length = *(it + 1) - '0';
        Lens lens{label, focal_length};
        if (lens_it == box.end()) {
            box.insert(lens_it, std::move(lens));
        } else {
            std::swap(lens, *lens_it);
        }
    } else if (*it == '-') {
        // remove a lens
        if (lens_it != box.end()) {
            box.erase(lens_it);
        }
    }
}

int Hashmap::focusing_power() const {
    int power = 0;
    for (std::size_t box_idx = 0; box_idx < boxes.size(); ++box_idx) {
        const std::list<Lens> &box = boxes[box_idx];
        int slot = 1;
        for (auto lens = box.begin(); lens != box.end(); ++lens, ++slot) {
            power += (box_idx + 1) * slot * lens->focal_length;
        }
    }
    return power;
}

} // namespace aoc::day15

#endif /* end of include guard: DAY_HPP_SD0ITZCR */
