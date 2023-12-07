/******************************************************************************
 * File:        test07.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-07
 *****************************************************************************/

#include <compare>  // for strong_ordering
#include <iostream> // for ostream

std::ostream &operator<<(std::ostream &os, const std::strong_ordering &value) {
    if (value < 0) {
        os << "less";
    } else if (value > 0) {
        os << "greater";
    } else {
        os << "equal";
    }
    return os;
}

#include "day07.hpp"
#include "unit_test/unit_test.hpp"
#include <cstddef> // for size_t
#include <sstream> // for istringstream

namespace unit_test {
template <>
struct argument_traits<aoc::day07::Hand>
    : detail::base_argument_traits<aoc::day07::Hand> {};
template <>
struct argument_traits<std::strong_ordering>
    : detail::base_argument_traits<std::strong_ordering> {};
} // namespace unit_test

namespace aoc::day07::test {

std::vector<Hand> read_hands_helper(const std::string &text) {
    std::istringstream ss{text};
    return read_hands(ss);
}

std::size_t test_read_hands() {
    unit_test::PureTest test("test07::test_read_hands", &read_hands_helper);
    Hand hand1{Card::THREE, Card::TWO, Card::TEN, Card::THREE, Card::KING, 765};
    Hand hand2{Card::TEN, Card::FIVE, Card::FIVE, Card::JACK, Card::FIVE, 684};
    Hand hand3{Card::THREE, Card::TWO, Card::TEN, Card::THREE, Card::KING, 765};
    test("32T3K 765", {hand1});
    test("32T3K 765\n", {hand1});
    test("32T3K 765\nT55J5 684", {hand1, hand2});
    test("32T3K 765\nT55J5 684\n", {hand1, hand2});
    test.done();
    return test.num_failed();
}

std::size_t test_hand_ordering() {
    unit_test::MethodTest<std::compare_three_way, std::strong_ordering,
                          const Hand &, const Hand &>
        test("test07::test_hand_sorting", &std::compare_three_way::operator());
    Hand hand1{Card::THREE, Card::TWO, Card::TEN, Card::THREE, Card::KING, 765};
    Hand hand2{Card::TEN, Card::FIVE, Card::FIVE, Card::JACK, Card::FIVE, 684};
    Hand hand3{Card::KING, Card::KING, Card::SIX, Card::SEVEN, Card::SEVEN, 28};
    Hand hand4{Card::KING, Card::TEN, Card::JACK, Card::JACK, Card::TEN, 220};
    Hand hand5{Card::QUEEN, Card::QUEEN, Card::QUEEN,
               Card::JACK,  Card::ACE,   483};

    // order: [1, 4, 3, 2, 5]
    test(hand1, hand1, std::strong_ordering::equal);
    test(hand1, hand2, std::strong_ordering::less);
    test(hand1, hand3, std::strong_ordering::less);
    test(hand1, hand4, std::strong_ordering::less);
    test(hand1, hand5, std::strong_ordering::less);

    test(hand2, hand2, std::strong_ordering::equal);
    test(hand2, hand3, std::strong_ordering::greater);
    test(hand2, hand4, std::strong_ordering::greater);
    test(hand2, hand5, std::strong_ordering::less);

    test(hand3, hand3, std::strong_ordering::equal);
    test(hand3, hand4, std::strong_ordering::greater);
    test(hand3, hand5, std::strong_ordering::less);

    test(hand4, hand4, std::strong_ordering::equal);
    test(hand4, hand5, std::strong_ordering::less);

    test(hand5, hand5, std::strong_ordering::equal);

    test.done();
    return test.num_failed();
}
} // namespace aoc::day07::test

int main() {
    std::size_t failed_count = 0;
    failed_count += aoc::day07::test::test_read_hands();
    failed_count += aoc::day07::test::test_hand_ordering();
    return unit_test::fix_exit_code(failed_count);
}
