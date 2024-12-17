/******************************************************************************
 * File:        day11.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-11
 *****************************************************************************/

#ifndef DAY11_HPP_UOIOTEZD
#define DAY11_HPP_UOIOTEZD

#include "lib.hpp"       // for DEBUG, FAST
#include "util/math.hpp" // for num_digits, powi
#include <algorithm>     // for count_if
#include <cassert>       // for assert
#include <cstddef>       // for size_t
#include <cstdint>       // for int64_t
#include <functional>    // for plus
#include <iostream>      // for istream, ostream, cerr
#include <map>           // for map
#include <numeric>       // for transform_reduce
#include <utility>       // for move
#include <variant>       // for variant, get, holds_alternative, visit
#include <vector>        // for vector

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
 *
 * Even though it's specifically called out in the description, the order of
 * the stones doesn't actually matter. Both answers are simply a stone count,
 * and the update rules are context-free.
 */
class StoneGroup {
    std::vector<Stone> stones{};

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
    void push_back(stone_value_t value, HistoryData &history);

    void blink(HistoryData &history);
    std::size_t count(const HistoryData &history) const;
    /// returns the total count of non-reference stones, for debugging purposes
    std::size_t eval_count() const;

    friend std::ostream &operator<<(std::ostream &, const StoneGroup &);
};

/**
 * Tracks the number of stones present after each blink when starting from a
 * single number.
 */
class HistoryData {
    struct HistoryEntry {
        StoneGroup group{};
        std::vector<std::size_t> counts{};
    };
    std::map<int, HistoryEntry> entries{};
    std::map<int, HistoryEntry> pending_entries{};
    bool updating_self = false;

    void add_entry(stone_value_t value);

  public:
    HistoryData();
    std::size_t count(const HistoryRef &ref) const;
    /// returns the total count of non-reference stones, for debugging purposes
    std::size_t eval_count() const;

    void blink();
    Stone get_stone(stone_value_t value);
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
    /// returns the total count of non-reference stones, for debugging purposes
    std::size_t eval_count() const;

    static Stones read(std::istream &is);

    friend std::ostream &operator<<(std::ostream &, const Stones &);
};

/***************************
 *  construction routines  *
 ***************************/

HistoryData::HistoryData() {
    // initialize single digits, skipping 0 as it immediately turns into 1
    for (stone_value_t i = 1; i <= 9; ++i) {
        add_entry(i);
    }
}

void StoneGroup::push_back(stone_value_t value, HistoryData &history) {
    return stones.push_back(history.get_stone(value));
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

void HistoryData::add_entry(stone_value_t value) {
    StoneGroup group{};
    group.push_back(value);
    HistoryEntry entry{std::move(group), {1}};
    if (updating_self) {
        pending_entries[value] = std::move(entry);
    } else {
        entries[value] = std::move(entry);
    }
}

Stone HistoryData::get_stone(stone_value_t value) {
    if (entries.contains(value) || pending_entries.contains(value)) {
        return HistoryRef{static_cast<int>(value)};
    }
    if (value >= 100 && value < 1000) {
        // store 3 digit numbers as we encounter them
        add_entry(value);
        return HistoryRef{static_cast<int>(value)};
    }
    return value;
}

// Main update routine.
void StoneGroup::blink(HistoryData &history) {
    std::size_t orig_size = stones.size();
    for (std::size_t i = 0; i < orig_size; ++i) {
        Stone &stone = stones[i];
        if (std::holds_alternative<HistoryRef>(stone)) {
            ++std::get<HistoryRef>(stone).blinks;
            continue;
        }
        stone_value_t value = std::get<stone_value_t>(stone);
        if (value == 0) {
            stone = history.get_stone(1);
        } else {
            int ndigits = math::num_digits(value);
            if (ndigits % 2 == 0) {
                stone_value_t modulus =
                    math::powi<stone_value_t>(10, ndigits / 2);
                stone = history.get_stone(value / modulus);
                stones.push_back(history.get_stone(value % modulus));
            } else {
                stone = history.get_stone(value * 2024);
            }
        }
    }
}

void HistoryData::blink() {
    // update each of the tracked groups
    updating_self = true;
    for (auto &[key, entry] : entries) {
        auto old_eval_count = entry.group.eval_count();
        entry.group.blink(*this);
        // record stone counts
        entry.counts.push_back(entry.group.count(*this));
        if constexpr (aoc::DEBUG) {
            if (entry.group.eval_count() == 0 && old_eval_count != 0) {
                std::cerr << "tracked group " << key
                          << " reached static state after "
                          << entry.counts.size() - 2
                          << " blinks: " << entry.group << "\n";
            }
        }
    }
    updating_self = false;
    // blink new entries once to break dependencies -- otherwise the program
    // may crash if a smaller number references a newly-added larger number,
    // since the smaller number would need the larger number to be updated
    // first
    for (auto &[key, entry] : pending_entries) {
        entry.group.blink(*this);
        entry.counts.push_back(entry.group.count(*this));
    }
    entries.merge(pending_entries);
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
    if (auto it = entries.find(ref.initial_value); it != entries.end()) {
        return it->second.counts.at(ref.blinks);
    }
    if constexpr (!aoc::FAST) {
        assert(ref.blinks == 0);
        assert(pending_entries.contains(ref.initial_value));
    }
    return 1;
}

std::size_t Stones::count() const { return group.count(history); }

std::size_t StoneGroup::eval_count() const {
    return std::count_if(
        stones.begin(), stones.end(),
        std::holds_alternative<stone_value_t, stone_value_t, HistoryRef>);
}

std::size_t HistoryData::eval_count() const {
    return std::transform_reduce(
        entries.begin(), entries.end(), 0L, std::plus<>{},
        [](const auto &value) { return value.second.group.eval_count(); });
}

std::size_t Stones::eval_count() const {
    return group.eval_count() + history.eval_count();
}

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
