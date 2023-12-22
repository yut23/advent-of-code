/******************************************************************************
 * File:        test12.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-15
 *****************************************************************************/

#include "day12.hpp" // IWYU pragma: associated
#include "unit_test/unit_test.hpp"
#include <cstddef> // for size_t
#include <sstream> // for istringstream, ostringstream
#include <string>  // for string
#include <vector>  // for vector

namespace aoc::day12::test {

ConditionRecord from_string(const std::string &text) {
    std::istringstream ss{text};
    std::vector<ConditionRecord> records = read_records(ss);
    return records[0];
}

std::size_t test_count_dp() {
    unit_test::PureTest test(
        "test12::test_count_dp", +[](const std::string &text) {
            return from_string(text).count_arrangements();
        });

    test("???.### 1,1,3", 1);
    test(".??..??...?##. 1,1,3", 4);
    test("?#?#?#?#?#?#?#? 1,3,1,6", 1);
    test("????.#...#... 4,1,1", 1);
    test("????.######..#####. 1,6,5", 4);
    test("?###???????? 3,2,1", 10);
    test("?###???????# 3,2,1", 4);
    test("??#???#?????.? 5,1,1", 12);
    test("#####.#.?? 5,1", 1);
    test("..#####.?? 5,1", 2);
    test("??#???#??? 5,1", 3);
    test(".#####.?? 5,1", 2);
    test("?#???#??? 5,1", 2);

    test.done();
    return test.num_failed();
}

std::size_t test_repeat() {
    unit_test::PureTest test(
        "test12::test_repeat", +[](const std::string &text, int count) {
            std::ostringstream oss{};
            oss << from_string(text).repeat(count);
            return oss.str();
        });

    test(".# 1", 5, ".#?.#?.#?.#?.# 1,1,1,1,1");
    test("???.### 1,1,3", 5,
         "???.###????.###????.###????.###????.### "
         "1,1,3,1,1,3,1,1,3,1,1,3,1,1,3");
    test(".#?. 2,1", 3, ".#?.?.#?.?.#?. 2,1,2,1,2,1");
    test(".?. 1", 1, ".?. 1");

    test.done();
    return test.num_failed();
}

} // namespace aoc::day12::test

int main() {
    std::size_t failed_count = 0;
    failed_count += aoc::day12::test::test_count_dp();
    failed_count += aoc::day12::test::test_repeat();
    return unit_test::fix_exit_code(failed_count);
}
