/******************************************************************************
 * File:        day11.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-11
 *****************************************************************************/

#ifndef DAY11_HPP_UOIOTEZD
#define DAY11_HPP_UOIOTEZD

#include <array>    // for array
#include <cstddef>  // for size_t
#include <cstdint>  // for int64_t
#include <iostream> // for istream, ostream
#include <list>     // for list
#include <string>   // for string, to_string, stoi
#include <variant>  // for variant, get, holds_alternative, visit
#include <vector>   // for vector

namespace aoc::day11 {

using stone_value_t = std::int64_t;

class HistoryData;

/**
 * A reference to a particular starting configuration from a HistoryData
 * struct after blinking a specific number of times.
 */
struct HistoryRef {
    int initial_value;
    int blinks = 0;
};

using Stone = std::variant<stone_value_t, HistoryRef>;

// helper for std::visit
template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
#if __cpp_deduction_guides < 201907L
// explicit deduction guide (not needed as of C++20 and Clang 17+)
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;
#endif

/**
 * Holds the state of a particular group of stones.
 */
class StoneGroup {
    std::list<Stone> stones{};

  public:
    using iterator = decltype(stones)::iterator;
    using const_iterator = decltype(stones)::const_iterator;

  private:
    iterator insert(const_iterator pos, stone_value_t value,
                    const HistoryData &history);

  public:
    /// Default constructor.
    StoneGroup() {}

    /**
     * Adds a stone with the given value to the end of the group. Will always
     * be a raw value, not a HistoryRef.
     */
    void push_back(stone_value_t value) { stones.push_back(value); }
    /**
     * Adds a stone with the given value to the end of the group. May be a
     * HistoryRef.
     */
    void push_back(stone_value_t value, const HistoryData &history) {
        insert(stones.end(), value, history);
    };

    void blink(const HistoryData &history);
    std::size_t count(const HistoryData &history) const;

    friend std::ostream &operator<<(std::ostream &, const StoneGroup &);
};

/**
 * Tracks the number of stones present after each blink when starting from a
 * single number.
 */
class HistoryData {
    // This currently only tracks the single digit numbers 1-9, but the public
    // interface is generic enough to allow for extension if needed.
    // 0 is skipped, as it immediately turns into 1. It still gets an
    // entry in the array so we don't have to shift indices.
    std::array<StoneGroup, 10> single_digits{};
    std::array<std::vector<std::size_t>, 10> counts{};

  public:
    HistoryData();
    std::size_t count(const HistoryRef &ref) const;

    void blink();
    Stone get_stone(stone_value_t value) const;
    void set_stone(Stone &stone, stone_value_t value) const;
};

/**
 * Everything needed to (efficiently) simulate the evolution of a particular
 * line of stones.
 */
class Stones {
    StoneGroup group;
    HistoryData history{};

  public:
    void blink();
    std::size_t count() const;

    static Stones read(std::istream &is);

    friend std::ostream &operator<<(std::ostream &, const Stones &);
};

/***************************
 *  construction routines  *
 ***************************/

HistoryData::HistoryData() {
    // initialize the digits skipping 0
    for (stone_value_t i = 1; i <= 9; ++i) {
        single_digits[i].push_back(i);
        counts[i].push_back(1);
    }
}

StoneGroup::iterator StoneGroup::insert(const_iterator it, stone_value_t value,
                                        const HistoryData &history) {
    return stones.insert(it, history.get_stone(value));
}

Stones Stones::read(std::istream &is) {
    Stones stones;
    stone_value_t value;
    while (is >> value) {
        stones.group.push_back(value, stones.history);
    }
    return stones;
}

/*********************
 *  update routines  *
 *********************/

Stone HistoryData::get_stone(stone_value_t value) const {
    if (value >= 1 && value <= 9) {
        return HistoryRef{static_cast<int>(value)};
    }
    return value;
}

void HistoryData::set_stone(Stone &stone, stone_value_t value) const {
    if (value >= 1 && value <= 9) {
        stone = HistoryRef{static_cast<int>(value)};
    }
    stone = value;
}

// Main update routine.
void StoneGroup::blink(const HistoryData &history) {
    for (auto it = stones.begin(); it != stones.end(); ++it) {
        if (std::holds_alternative<HistoryRef>(*it)) {
            ++std::get<HistoryRef>(*it).blinks;
            continue;
        }
        stone_value_t value = std::get<stone_value_t>(*it);
        if (value == 0) {
            history.set_stone(*it, 1);
        } else if (std::string digits = std::to_string(value);
                   digits.size() % 2 == 0) {
            int half_size = digits.size() / 2;
            insert(it, std::stoi(digits.substr(0, half_size)), history);
            *it = history.get_stone(std::stoi(digits.substr(half_size)));
        } else {
            *it = history.get_stone(value * 2024);
        }
    }
}

void HistoryData::blink() {
    // update each of the tracked groups
    for (auto &group : single_digits) {
        group.blink(*this);
    }
    // record stone counts
    for (std::size_t i = 1; i < single_digits.size(); ++i) {
        std::size_t c = single_digits[i].count(*this);
        counts[i].push_back(c);
    }
}

void Stones::blink() {
    history.blink();
    group.blink(history);
}

/***********************
 *  counting routines  *
 ***********************/
std::size_t StoneGroup::count(const HistoryData &history) const {
    std::size_t count = 0;
    for (const auto &stone : stones) {
        if (std::holds_alternative<HistoryRef>(stone)) {
            count += history.count(std::get<HistoryRef>(stone));
        } else {
            ++count;
        }
    }
    return count;
}

std::size_t HistoryData::count(const HistoryRef &ref) const {
    return counts[ref.initial_value].at(ref.blinks);
}

std::size_t Stones::count() const { return group.count(history); }

/***********************
 *  printing routines  *
 ***********************/
std::ostream &operator<<(std::ostream &os, const Stone &stone) {
    std::visit(overloaded{
                   [&](stone_value_t value) { os << value; },
                   [&](const HistoryRef &ref) {
                       os << "{" << ref.initial_value << "}+" << ref.blinks;
                   },
               },
               stone);
    return os;
}

std::ostream &operator<<(std::ostream &os, const StoneGroup &group) {
    for (auto it = group.stones.begin(); it != group.stones.end(); ++it) {
        if (it != group.stones.begin()) {
            os << ' ';
        }
        os << *it;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const Stones &stones) {
    os << stones.group;
    return os;
}

} // namespace aoc::day11

#endif /* end of include guard: DAY11_HPP_UOIOTEZD */
