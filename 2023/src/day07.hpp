/******************************************************************************
 * File:        day07.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-07
 *****************************************************************************/

#ifndef DAY07_HPP_QKDJV1MU
#define DAY07_HPP_QKDJV1MU

#include "lib.hpp"    // for read_vector
#include <algorithm>  // for sort
#include <array>      // for array
#include <cassert>    // for assert
#include <compare>    // for strong_ordering
#include <functional> // for greater
#include <iostream>   // for istream, ostream
#include <utility>    // for move
#include <vector>     // for vector

namespace aoc::day07 {

enum class Card : unsigned char {
    JOKER = 1,
    TWO = 2,
    THREE = 3,
    FOUR = 4,
    FIVE = 5,
    SIX = 6,
    SEVEN = 7,
    EIGHT = 8,
    NINE = 9,
    TEN = 10,
    JACK = 11,
    QUEEN = 12,
    KING = 13,
    ACE = 14,
};

std::istream &operator>>(std::istream &is, Card &card) {
    char ch;
    is >> ch;
    if (ch >= '2' && ch <= '9') {
        card = Card(ch - '0');
    } else {
        switch (ch) {
        case 'T':
            card = Card::TEN;
            break;
        case 'J':
            card = Card::JACK;
            break;
        case 'Q':
            card = Card::QUEEN;
            break;
        case 'K':
            card = Card::KING;
            break;
        case 'A':
            card = Card::ACE;
            break;
        }
    }
    return is;
}

std::ostream &operator<<(std::ostream &os, const Card &card) {
    switch (card) {
    case Card::TWO:
    case Card::THREE:
    case Card::FOUR:
    case Card::FIVE:
    case Card::SIX:
    case Card::SEVEN:
    case Card::EIGHT:
    case Card::NINE:
        os << static_cast<unsigned char>(card);
        break;
    case Card::TEN:
        os << 'T';
        break;
    case Card::JACK:
        os << 'J';
        break;
    case Card::QUEEN:
        os << 'Q';
        break;
    case Card::KING:
        os << 'K';
        break;
    case Card::ACE:
        os << 'A';
        break;
    case Card::JOKER:
        os << '?';
        break;
    }
    return os;
}

enum class HandType {
    HIGH_CARD,       // {1, 1, 1, 1, 1}
    ONE_PAIR,        // {2, 1, 1, 1}
    TWO_PAIR,        // {2, 2, 1}
    THREE_OF_A_KIND, // {3, 1, 1}
    FULL_HOUSE,      // {3, 2}
    FOUR_OF_A_KIND,  // {4, 1}
    FIVE_OF_A_KIND,  // {5}
};
std::ostream &operator<<(std::ostream &os, const HandType &hand_type) {
    switch (hand_type) {
    case HandType::HIGH_CARD:
        os << "high card";
        break;
    case HandType::ONE_PAIR:
        os << "one pair";
        break;
    case HandType::TWO_PAIR:
        os << "two_pair";
        break;
    case HandType::THREE_OF_A_KIND:
        os << "three of a kind";
        break;
    case HandType::FULL_HOUSE:
        os << "full house";
        break;
    case HandType::FOUR_OF_A_KIND:
        os << "four of a kind";
        break;
    case HandType::FIVE_OF_A_KIND:
        os << "five of a kind";
        break;
    }
    return os;
}

HandType identify_hand(const std::array<Card, 5> &cards) {
    std::vector<int> counts(static_cast<unsigned char>(Card::ACE) + 1);
    for (const Card card : cards) {
        ++counts[static_cast<unsigned char>(card)];
    }
    // The hand type ranking is equivalent a lexicographic ordering of
    // `counts`, so jokers should take the same value as the most common card.
    int joker_count = 0;
    // set the joker count to 0 in counts, so it doesn't get included in the
    // ranking
    std::swap(joker_count, counts[static_cast<unsigned char>(Card::JOKER)]);
    // sort card counts in descending order
    std::ranges::sort(counts, std::ranges::greater{});

    counts[0] += joker_count;
    switch (counts[0]) {
    case 5:
        return HandType::FIVE_OF_A_KIND;
    case 4:
        return HandType::FOUR_OF_A_KIND;
    case 3:
        switch (counts[1]) {
        case 2:
            return HandType::FULL_HOUSE;
        case 1:
            return HandType::THREE_OF_A_KIND;
        }
        break;
    case 2:
        switch (counts[1]) {
        case 2:
            return HandType::TWO_PAIR;
        case 1:
            return HandType::ONE_PAIR;
        }
        break;
    case 1:
        return HandType::HIGH_CARD;
    }
    assert(false);
}

struct Hand {
    std::array<Card, 5> cards;
    int bid;
    HandType hand_type;

    Hand() : Hand({}, 0) {}
    Hand(const std::array<Card, 5> &cards, int bid)
        : cards(cards), bid(bid), hand_type(identify_hand(cards)){};
    Hand(const Card &card1, const Card &card2, const Card &card3,
         const Card &card4, const Card &card5, int bid)
        : Hand({card1, card2, card3, card4, card5}, bid){};

    void jacks_to_jokers() {
        // replace jacks with jokers
        std::ranges::replace(cards, Card::JACK, Card::JOKER);
        // recalculate hand type
        hand_type = identify_hand(cards);
    }

    // N.B.: this is only implicitly defaulted if operator<=> is defaulted.
    bool operator==(const Hand &) const = default;
    std::strong_ordering operator<=>(const Hand &other) const {
        if (auto cmp = hand_type <=> other.hand_type; cmp != 0)
            return cmp;
        if (auto cmp = cards <=> other.cards; cmp != 0)
            return cmp;
        return bid <=> other.bid;
    }
};

std::istream &operator>>(std::istream &is, Hand &hand) {
    std::array<Card, 5> cards;
    for (auto &card : cards) {
        is >> card;
    }
    int bid;
    is >> bid;
    if (is) {
        hand = Hand(std::move(cards), bid);
    }
    return is;
}

std::ostream &operator<<(std::ostream &os, const Hand &hand) {
    for (const auto &card : hand.cards) {
        os << card;
    }
    os << ' ' << hand.bid << " (hand_type=" << hand.hand_type << ")";
    return os;
}

std::vector<Hand> read_hands(std::istream &is) {
    return aoc::read_vector<Hand>(is);
}

} // namespace aoc::day07

#endif /* end of include guard: DAY07_HPP_QKDJV1MU */
