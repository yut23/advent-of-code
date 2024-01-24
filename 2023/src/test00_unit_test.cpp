/******************************************************************************
 * File:        test00_unit_test.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-01-23
 * Description: Tests of the unit test framework itself.
 *****************************************************************************/

#include "unit_test/unit_test.hpp"

#include <compare>       // for strong_ordering
#include <cstddef>       // for size_t
#include <string>        // for string
#include <unordered_map> // for unordered_map
#include <vector>        // for vector

namespace unit_test::test {

std::size_t test_strong_ordering() {
    unit_test::PureTest test(
        "unit_test::strong_ordering",
        +[](const std::vector<int> &vec1, const std::vector<int> &vec2)
            -> std::strong_ordering { return vec1 <=> vec2; });
    test({1, 2}, {1, 2}, std::strong_ordering::equal);
    test({1, 1}, {1, 2}, std::strong_ordering::less);
    test({2, 2}, {1, 2}, std::strong_ordering::greater);
    test({1, 2, 3}, {1, 2}, std::strong_ordering::greater);
    test({1, 1, 3}, {1, 2}, std::strong_ordering::less, "note");
    return test.done(), test.num_failed();
}

std::size_t test_unordered_map() {
    const auto count_chars =
        +[](const std::string &s) -> std::unordered_map<char, int> {
        std::unordered_map<char, int> counts;
        for (char ch : s) {
            ++counts[ch];
        }
        return counts;
    };
    unit_test::PureTest test("unit_test::unordered_map", count_chars);
    test("abc", {{'a', 1}, {'b', 1}, {'c', 1}});
    test("banana", {{'a', 3}, {'b', 1}, {'n', 2}});
    return test.done(), test.num_failed();
}

} // namespace unit_test::test

int main() {
    std::size_t failed_count = 0;
    failed_count += unit_test::test::test_strong_ordering();
    failed_count += unit_test::test::test_unordered_map();
    return unit_test::fix_exit_code(failed_count);
}
