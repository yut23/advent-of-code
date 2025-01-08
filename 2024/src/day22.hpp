/******************************************************************************
 * File:        day22.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-22
 *****************************************************************************/

#ifndef DAY22_HPP_A3Y597BB
#define DAY22_HPP_A3Y597BB

#include "lib.hpp"       // for as_number, DEBUG
#include "util/math.hpp" // for powi

#include <algorithm> // for max_element
#include <compare>   // for strong_ordering
#include <cstddef>   // for size_t
#include <cstdint>   // for int8_t, uint32_t, uint64_t
#include <iostream>  // for cerr, istream, ostream
#include <iterator>  // for distance
#include <vector>    // for vector

namespace aoc::day22 {

struct ChangeSequence {
    static constexpr std::uint32_t BASE = 19;
    static constexpr std::uint32_t MODULUS = aoc::math::powi(BASE, 4);

    std::uint32_t value{};

    std::int8_t operator[](std::size_t index) const;

    void add_price_change(std::int8_t new_change);

    // implicit conversion to an integer index
    operator std::size_t() const { return value; }
};

std::int8_t ChangeSequence::operator[](std::size_t index) const {
    return static_cast<std::int8_t>((value / aoc::math::powi(BASE, 3 - index)) %
                                    BASE) -
           9;
}

void ChangeSequence::add_price_change(std::int8_t new_change) {
    value *= BASE;
    value += static_cast<std::uint8_t>(new_change + 9);
    value %= MODULUS;
}

std::ostream &operator<<(std::ostream &os, const ChangeSequence &changes) {
    os << aoc::as_number(changes[0]) << ',' << aoc::as_number(changes[1]) << ','
       << aoc::as_number(changes[2]) << ',' << aoc::as_number(changes[3]);
    return os;
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
    curr_changes.add_price_change(new_price - old_price);
}

class MonkeyMarket {
    std::vector<Buyer> buyers;

    struct PriceEntry {
        // running total for each change sequence
        std::uint32_t total_bananas = 0;

      private:
        // last monkey to encounter this sequence
        std::uint32_t last_monkey_plus_1 = 0;

      public:
        void update(std::int8_t price, std::size_t monkey_index);

        std::strong_ordering
        operator<=>(const PriceEntry &other) const = default;
    };
    std::vector<PriceEntry> price_lookup{ChangeSequence::MODULUS};

  public:
    void evolve(int iter);
    std::uint64_t get_sum() const;
    int find_best_sell_sequence() const;

    static MonkeyMarket read(std::istream &);
};

void MonkeyMarket::PriceEntry::update(std::int8_t price,
                                      std::size_t monkey_index) {
    // this assumes we simulate the buyers individually (i.e. not interleaving
    // evolve calls)
    if (last_monkey_plus_1 != monkey_index + 1) {
        total_bananas += price;
        last_monkey_plus_1 = monkey_index + 1;
    }
}

void MonkeyMarket::evolve(int num_iters) {
    for (std::size_t i = 0; i < buyers.size(); ++i) {
        Buyer &buyer = buyers[i];
        buyer.evolve();
        buyer.evolve();
        buyer.evolve();
        for (int iter = 3; iter < num_iters; ++iter) {
            buyer.evolve();
            price_lookup[buyer.curr_changes].update(buyer.price(), i);
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
    auto it = std::max_element(price_lookup.begin(), price_lookup.end());
    if constexpr (aoc::DEBUG) {
        ChangeSequence changes{static_cast<std::uint32_t>(
            std::distance(price_lookup.begin(), it))};
        std::cerr << "best sequence: " << changes << "\n";
    }
    return it->total_bananas;
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
