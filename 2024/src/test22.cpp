/******************************************************************************
 * File:        test22.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-01-07
 *****************************************************************************/

#include "day22.hpp" // IWYU pragma: associated

#include "unit_test/unit_test.hpp"

#include <cstddef>         // for size_t
#include <source_location> // for source_location

namespace aoc::day22::test {
std::size_t test_ChangeSequence() {
    unit_test::TestSuite suite("aoc::day22::ChangeSequence");

    using namespace unit_test::checks;
    constexpr auto check_valid = [](const ChangeSequence &changes,
                                    std::source_location loc =
                                        std::source_location::current()) {
        check(
            changes[0] >= -9 && changes[0] <= 9,
            [&](auto &os) {
                os << "first value out of range: "
                   << aoc::as_number(changes[0]);
            },
            loc);
        check(
            changes[1] >= -9 && changes[1] <= 9,
            [&](auto &os) {
                os << "second value out of range: "
                   << aoc::as_number(changes[1]);
            },
            loc);
        check(
            changes[2] >= -9 && changes[2] <= 9,
            [&](auto &os) {
                os << "third value out of range: "
                   << aoc::as_number(changes[2]);
            },
            loc);
        check(
            changes[3] >= -9 && changes[3] <= 9,
            [&](auto &os) {
                os << "fourth value out of range: "
                   << aoc::as_number(changes[3]);
            },
            loc);
    };

    constexpr auto check_values = [](const ChangeSequence &changes, int first,
                                     int second, int third, int fourth,
                                     std::source_location loc =
                                         std::source_location::current()) {
        check_equal(
            changes[0], first,
            [&](auto &os) {
                os << "first value incorrect (" << changes.value << ")";
            },
            loc);
        check_equal(
            changes[1], second,
            [&](auto &os) {
                os << "second value incorrect (" << changes.value << ')';
            },
            loc);
        check_equal(
            changes[2], third,
            [&](auto &os) {
                os << "third value incorrect (" << changes.value << ')';
            },
            loc);
        check_equal(
            changes[3], fourth,
            [&](auto &os) {
                os << "fourth value incorrect (" << changes.value << ')';
            },
            loc);
        check(changes.value < aoc::math::powi(changes.BASE, 4), [&](auto &os) {
            os << "raw value is greater than BASE**4 (" << changes.value << ')';
        });
    };

    suite.test("add_price_change", [&]() {
        ChangeSequence changes{};
        check_valid(changes);
        changes.add_price_change(-1);
        check_valid(changes);
        changes.add_price_change(1);
        check_valid(changes);
        changes.add_price_change(9);
        check_valid(changes);
        changes.add_price_change(-9);
        check_valid(changes);
        check_values(changes, -1, 1, 9, -9);

        changes.add_price_change(2);
        check_values(changes, 1, 9, -9, 2);

        changes.add_price_change(3);
        check_values(changes, 9, -9, 2, 3);

        changes.add_price_change(4);
        check_values(changes, -9, 2, 3, 4);

        changes.add_price_change(8);
        check_values(changes, 2, 3, 4, 8);
    });
    return suite.done(), suite.num_failed();
}
} // namespace aoc::day22::test

int main() {
    std::size_t num_failed = 0;
    num_failed += aoc::day22::test::test_ChangeSequence();
    return unit_test::fix_exit_code(num_failed);
}
