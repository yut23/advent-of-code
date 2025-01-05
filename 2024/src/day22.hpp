/******************************************************************************
 * File:        day22.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-22
 *****************************************************************************/

#ifndef DAY22_HPP_A3Y597BB
#define DAY22_HPP_A3Y597BB

#include "lib.hpp"                    // for DEBUG
#include "unit_test/pretty_print.hpp" // for repr
#include "util/hash.hpp"              // for make_hash

#include <algorithm>     // for ranges::max_element
#include <array>         // for array
#include <cstddef>       // for size_t
#include <cstdint>       // for uint8_t, uint32_t, uint64_t
#include <functional>    // for hash
#include <iomanip>       // for setw
#include <iostream>      // for cerr
#include <unordered_map> // for unordered_map
#include <utility>       // for pair (unordered_map)

namespace aoc::day22 {
struct ChangeSequence : public std::array<std::int8_t, 4> {};
} // namespace aoc::day22

template <>
struct std::hash<aoc::day22::ChangeSequence> {
    std::size_t
    operator()(const aoc::day22::ChangeSequence &changes) const noexcept {
        // random number (hexdump -n8 -e '"0x" 8/1 "%02x" "ull\n"'</dev/urandom)
        std::size_t seed = 0xc17a07615bebc283ull;
        util::make_hash(seed, changes[0], changes[1], changes[2], changes[3]);
        return seed;
    }
};

namespace aoc::day22 {

ChangeSequence add_price_change(const ChangeSequence &changes,
                                std::int8_t new_change) {
    return ChangeSequence{changes[1], changes[2], changes[3], new_change};
}

class PriceSequence {
    std::uint32_t secret;
    int monkey_number;
    ChangeSequence curr_changes{};
    static std::unordered_map<ChangeSequence,
                              std::unordered_map<int, std::int8_t>>
        sequence_prices;

    void xorshift(int shift);

  public:
    explicit PriceSequence(std::uint32_t initial_secret, int monkey_number)
        : secret(initial_secret), monkey_number(monkey_number) {}

    void evolve(int iter);
    std::uint32_t get_secret() const { return secret; }
    std::int8_t get_price() const { return secret % 10; }

    static int find_best_sell_sequence();
};

void PriceSequence::xorshift(int shift) {
    std::uint32_t tmp = secret;
    if (shift < 0) {
        tmp >>= -shift;
    } else {
        tmp <<= shift;
    }
    secret = (secret ^ tmp) & 0xFF'FF'FF;
}

void PriceSequence::evolve(int iter) {
    // save previous price
    std::int8_t old_price = secret % 10;
    xorshift(+6);
    xorshift(-5);
    xorshift(+11);
    std::int8_t new_price = secret % 10;
    curr_changes = add_price_change(curr_changes, new_price - old_price);
    if (iter >= 3) {
        // try_emplace does nothing if the key already exists
        sequence_prices[curr_changes].try_emplace(monkey_number, new_price);
    }
}

int PriceSequence::find_best_sell_sequence() {
    const auto sell_sequence_value = [](const auto &entry) -> int {
        int total = 0;
        for (const auto &[_, price] : entry.second) {
            total += price;
        }
        return total;
    };
    auto it =
        std::ranges::max_element(sequence_prices, {}, sell_sequence_value);
    if constexpr (aoc::DEBUG) {
        std::cerr << "best sequence: "
                  << pretty_print::repr<std::array<std::int8_t, 4>>(
                         it->first, {.char_as_number = true})
                  << "\n";
        std::cerr << "prices:\n";
        for (const auto &[monkey, price] : it->second) {
            std::cerr << std::setw(5) << monkey << ": " << aoc::as_number(price)
                      << "\n";
        }
    }
    return sell_sequence_value(*it);
}

std::unordered_map<ChangeSequence, std::unordered_map<int, std::int8_t>>
    PriceSequence::sequence_prices{};

} // namespace aoc::day22

#endif /* end of include guard: DAY22_HPP_A3Y597BB */
