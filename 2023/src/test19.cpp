/******************************************************************************
 * File:        test19.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-19
 *****************************************************************************/

#include "day19.hpp" // IWYU pragma: associated
#include "unit_test/unit_test.hpp"
#include <cstddef> // for size_t
#include <sstream> // for istringstream, ostringstream
#include <string>  // for string

// IWYU pragma: no_forward_declare unit_test::argument_traits
namespace unit_test {
template <>
struct argument_traits<aoc::day19::Condition>
    : detail::base_argument_traits<aoc::day19::Condition> {};
} // namespace unit_test

namespace aoc::day19::test {

std::string invert_condition_helper(const std::string &s) {
    Condition cond;
    {
        std::istringstream iss{s};
        if (!(iss >> cond)) {
            return "";
        }
    }
    std::ostringstream oss{};
    oss << cond.invert();
    return oss.str();
}

std::size_t test_condition_insertion() {
    unit_test::PureTest test(
        "day19::test_condition_insertion",
        +[](const Condition &cond) -> std::string {
            std::ostringstream ss{};
            ss << cond;
            return ss.str();
        });

    test(Condition{true, 0, 10}, "x>10");
    test(Condition{false, 1, 1}, "m<1");
    test(Condition{false, 2, 1000}, "a<1000");
    test(Condition{true, 3, 3821}, "s>3821");

    return test.done(), test.num_failed();
}

std::size_t test_condition_extraction() {
    unit_test::PureTest test(
        "day19::test_condition_extraction",
        +[](const std::string &s) -> Condition {
            std::istringstream ss{s};
            Condition cond{};
            ss >> cond;
            return cond;
        });

    test("x>10", Condition{true, 0, 10});
    test("m<1", Condition{false, 1, 1});
    test("a<1000", Condition{false, 2, 1000});
    test("s>3821", Condition{true, 3, 3821});

    return test.done(), test.num_failed();
}

std::size_t test_invert_condition() {
    unit_test::PureTest test("day19::test_invert_condition",
                             invert_condition_helper);

    test("a>10", "a<11");
    test("m<1", "m>0");
    test("x<51", "x>50");

    return test.done(), test.num_failed();
}

} // namespace aoc::day19::test

int main() {
    std::size_t failed_count = 0;
    failed_count += aoc::day19::test::test_condition_extraction();
    failed_count += aoc::day19::test::test_condition_insertion();
    failed_count += aoc::day19::test::test_invert_condition();
    return unit_test::fix_exit_code(failed_count);
}
