/******************************************************************************
 * File:        test05.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-05
 *****************************************************************************/

#include "day05.hpp"
#include "unit_test/unit_test.hpp"
#include <cassert>  // for assert
#include <cstddef>  // for size_t
#include <iostream> // for cerr, endl
#include <iterator> // for distance
#include <numeric>  // for iota
#include <vector>   // for vector

namespace unit_test {
template <>
struct argument_traits<aoc::day05::ConversionMap>
    : detail::base_argument_traits<aoc::day05::ConversionMap> {};
} // namespace unit_test

namespace aoc::day05::test {

std::size_t test_mapping() {
    ConversionMap map;
    map.add_entry(MapEntry(50, 98, 2));
    map.add_entry(MapEntry(52, 50, 48));
    unit_test::MethodTest<ConversionMap, long, long> test(
        "test05::test_mapping", map, &ConversionMap::apply);

    std::vector<long> input_vec(110);
    std::iota(input_vec.begin(), input_vec.end(), 0);
    std::vector<long> expected_vec(110);
    auto start = expected_vec.begin();
    std::iota(start, start + 50, 0);
    start += 50;
    std::iota(start, start + 48, 52);
    start += 48;
    std::iota(start, start + 2, 50);
    start += 2;
    std::iota(start, expected_vec.end(),
              std::distance(expected_vec.begin(), start));

    for (std::size_t i = 0; i < input_vec.size(); ++i) {
        test.run_on_args(input_vec[i], expected_vec[i]);
    }
    test.done();
    return test.num_failed();
}

ConversionMap parse_map(const std::string &text) {
    std::istringstream ss{text};
    return ConversionMap::read(ss);
}

std::size_t test_map_input() {
    std::istringstream ss{};
    unit_test::PureTest test("test05::test_map_input", &parse_map);
    test.run_on_args("seed-to-soil map:\n50 98 2\n52 50 48\n",
                     ConversionMap("seed-to-soil", std::vector<MapEntry>{
                                                       MapEntry(52, 50, 48),
                                                       MapEntry(50, 98, 2),
                                                   }));
    test.run_on_args(
        "soil-to-fertilizer map:\n0 15 37\n37 52 2\n39 0 15\n",
        ConversionMap("soil-to-fertilizer", std::vector<MapEntry>{
                                                MapEntry(39, 0, 15),
                                                MapEntry(0, 15, 37),
                                                MapEntry(37, 52, 2),
                                            }));
    test.done();
    return test.num_failed();
}
} // namespace aoc::day05::test

int main() {
    std::size_t failed_count = 0;
    failed_count += aoc::day05::test::test_mapping();
    failed_count += aoc::day05::test::test_map_input();
    return unit_test::fix_exit_code(failed_count);
}
