/******************************************************************************
 * File:        test_math.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-11
 *****************************************************************************/

#include "util/math.hpp" // IWYU pragma: associated

#include "unit_test/pretty_print.hpp" // for repr
#include "unit_test/unit_test.hpp"
#include "util/util.hpp" // for demangle

#include <cstddef>  // for size_t
#include <cstdint>  // for int64_t, uint64_t
#include <iostream> // for cout
#include <string>   // for string
// IWYU pragma: no_include <typeinfo>  // for type_info (util::demangle)

namespace aoc::math::test {

template <typename IntegerT>
std::size_t test_powers_of_10() {
    const std::string type_name = util::demangle(typeid(IntegerT).name());
    unit_test::PureTest test(
        "aoc::math::gen_powers_of_10<" + type_name + ">",
        +[](IntegerT x) -> IntegerT { return x; });

    constexpr auto POWERS_OF_10 = aoc::math::gen_powers_of_10<IntegerT>();
    std::size_t size = POWERS_OF_10.size();
    IntegerT max = std::numeric_limits<IntegerT>::max();
    int i = 0;
    IntegerT x = 10;
    while (x <= max / 10) {
        test(POWERS_OF_10.at(i), x,
             "incorrect element at index " + std::to_string(i));
        x *= 10;
        ++i;
    }
    for (std::size_t i = 2; i < size; ++i) {
        test(POWERS_OF_10[i], POWERS_OF_10[i - 1] * 10,
             "ratio between elements at index " + std::to_string(i) +
                 " should be 10");
    }
    test(POWERS_OF_10[size - 1] / 10, POWERS_OF_10[size - 2],
         "ratio between elements at index " + std::to_string(size - 2) +
             " should be 10");

    test.done();
    if (test.num_failed() > 0) {
        std::cout << "POWERS_OF_10: " << pretty_print::repr(POWERS_OF_10)
                  << "\n";
    }
    return test.num_failed();
}

template <class IntegerT>
std::size_t test_num_digits() {
    const std::string type_name = util::demangle(typeid(IntegerT).name());
    unit_test::PureTest test("aoc::math::num_digits<" + type_name + ">",
                             &aoc::math::num_digits<IntegerT>);

    test(1, 1);
    for (unsigned int i = 0; i <= std::numeric_limits<IntegerT>::digits10;
         ++i) {
        IntegerT p10 = powi(static_cast<IntegerT>(10), i);
        if (p10 > 1) {
            test(p10 - 1, i);
        }
        test(p10, i + 1);
        test(p10 + 1, i + 1);
    }
    test(std::numeric_limits<IntegerT>::max(),
         std::numeric_limits<IntegerT>::digits10 + 1);

    return test.done(), test.num_failed();
}

} // namespace aoc::math::test

int main() {
    std::size_t failed_count = 0;
    failed_count += aoc::math::test::test_powers_of_10<int>();
    failed_count += aoc::math::test::test_powers_of_10<unsigned int>();
    failed_count += aoc::math::test::test_powers_of_10<std::int64_t>();
    failed_count += aoc::math::test::test_powers_of_10<std::uint64_t>();
    failed_count += aoc::math::test::test_num_digits<int>();
    failed_count += aoc::math::test::test_num_digits<unsigned int>();
    failed_count += aoc::math::test::test_num_digits<std::int64_t>();
    failed_count += aoc::math::test::test_num_digits<std::uint64_t>();
    return unit_test::fix_exit_code(failed_count);
}
