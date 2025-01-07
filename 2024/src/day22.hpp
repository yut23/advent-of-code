/******************************************************************************
 * File:        day22.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-22
 *****************************************************************************/

#ifndef DAY22_HPP_A3Y597BB
#define DAY22_HPP_A3Y597BB

#include "lib.hpp"                    // for as_number, DEBUG
#include "unit_test/pretty_print.hpp" // for repr
#include "util/hash.hpp"              // for make_hash

#include <algorithm>     // for ranges::max_element
#include <array>         // for array
#include <cstddef>       // for size_t
#include <cstdint>       // for int8_t, uint32_t, uint64_t
#include <functional>    // for plus, hash
#include <iomanip>       // for setw
#include <iostream>      // for cerr, istream
#include <tuple>         // for forward_as_tuple
#include <unordered_map> // for unordered_map
#include <utility>       // for pair (unordered_map), piecewise_construct
#include <vector>        // for vector

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

class Buyer {
    std::uint32_t secret;
    ChangeSequence curr_changes{};

    void xorshift(int shift);

  public:
    explicit Buyer(std::uint32_t initial_secret) : secret(initial_secret) {}

    void evolve();
    std::int8_t price() const { return secret % 10; }

    friend class MonkeyMarket;
};

void Buyer::xorshift(int shift) {
    std::uint32_t tmp = secret;
    if (shift < 0) {
        tmp >>= -shift;
    } else {
        tmp <<= shift;
    }
    secret = (secret ^ tmp) & 0xFF'FF'FF;
}

void Buyer::evolve() {
    // save previous price
    std::int8_t old_price = secret % 10;
    xorshift(+6);
    xorshift(-5);
    xorshift(+11);
    std::int8_t new_price = secret % 10;
    curr_changes = add_price_change(curr_changes, new_price - old_price);
}

class MonkeyMarket {
    std::vector<Buyer> buyers;
    std::unordered_map<ChangeSequence, std::vector<std::int8_t>>
        sequence_prices;

  public:
    void evolve(int iter);
    std::uint64_t get_sum() const;
    int find_best_sell_sequence() const;

    static MonkeyMarket read(std::istream &);
};

void MonkeyMarket::evolve(int iter) {
    for (std::size_t i = 0; i < buyers.size(); ++i) {
        Buyer &buyer = buyers[i];
        buyer.evolve();
        if (iter >= 3) {
            auto it = sequence_prices.find(buyer.curr_changes);
            if (it == sequence_prices.end()) {
                // fill price list with -1
                it = sequence_prices
                         .emplace(std::piecewise_construct,
                                  std::forward_as_tuple(buyer.curr_changes),
                                  std::forward_as_tuple(buyers.size(), -1))
                         .first;
                it->second[i] = buyer.price();
            } else if (it->second[i] == -1) {
                it->second[i] = buyer.price();
            }
        }
    }
}

std::uint64_t MonkeyMarket::get_sum() const {
    std::uint64_t sum = 0;
    for (const Buyer &buyer : buyers) {
        // cppcheck-suppress useStlAlgorithm
        sum += buyer.secret;
    }
    return sum;
}

int MonkeyMarket::find_best_sell_sequence() const {
    const auto sell_sequence_value = [](const auto &entry) -> int {
        int total = 0;
        for (int price : entry.second) {
            if (price != -1) {
                // cppcheck-suppress useStlAlgorithm
                total += price;
            }
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
        for (int monkey = 0; const auto price : it->second) {
            if (price != -1) {
                std::cerr << std::setw(5) << monkey << ": "
                          << aoc::as_number(price) << "\n";
            }
            ++monkey;
        }
    }
    return sell_sequence_value(*it);
}

MonkeyMarket MonkeyMarket::read(std::istream &is) {
    MonkeyMarket market;
    std::uint32_t secret;
    while (is >> secret) {
        market.buyers.emplace_back(secret);
    }
    return market;
}

} // namespace aoc::day22

#endif /* end of include guard: DAY22_HPP_A3Y597BB */
