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

#include <cstddef>          // for size_t
#include <cstdint>          // for int64_t, uint64_t
#include <initializer_list> // for initializer_list
#include <iostream>         // for cout
#include <string>           // for string
// IWYU pragma: no_include <typeinfo>  // for type_info (util::demangle)

namespace aoc::math {
std::ostream &print_repr(std::ostream &os, const extended_gcd_result &result,
                         const pretty_print::repr_state /*state*/) {
    os << "{.gcd=" << result.gcd << ", .bezout_a=" << result.bezout_a
       << ", .bezout_b=" << result.bezout_b
       << ", .quotient_a=" << result.quotient_a
       << ", .quotient_b=" << result.quotient_b << "}";
    return os;
}

std::ostream &print_repr(std::ostream &os, const CRT::crt_entry &entry,
                         const pretty_print::repr_state /*state*/) {
    os << "{.remainder=" << entry.remainder << ", .modulus=" << entry.modulus
       << "}";
    return os;
}

} // namespace aoc::math

template <>
struct pretty_print::has_custom_print_repr<aoc::math::extended_gcd_result>
    : std::true_type {};

template <>
struct pretty_print::has_custom_print_repr<aoc::math::CRT::crt_entry>
    : std::true_type {};

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

std::size_t test_extended_gcd() {
    unit_test::PureTest test("aoc::math::extended_gcd",
                             &aoc::math::extended_gcd);

    test(240, 46,
         {.gcd = 2,
          .bezout_a = -9,
          .bezout_b = 47,
          .quotient_a = 240 / 2,
          .quotient_b = 46 / 2});
    test(15, 69,
         {.gcd = 3,
          .bezout_a = -9,
          .bezout_b = 2,
          .quotient_a = 15 / 3,
          .quotient_b = 69 / 3});
    test(12, 42,
         {.gcd = 6,
          .bezout_a = -3,
          .bezout_b = 1,
          .quotient_a = 12 / 6,
          .quotient_b = 42 / 6});
    test(5, 7,
         {.gcd = 1,
          .bezout_a = 3,
          .bezout_b = -2,
          .quotient_a = 5,
          .quotient_b = 7});

    return test.done(), test.num_failed();
}

std::size_t test_crt() {
    unit_test::TestSuite suite("aoc::math::CRT");

    const auto test = [&suite](const std::string &name,
                               std::initializer_list<CRT::crt_entry> entries,
                               long expected) {
        suite.test(name, [&entries, expected]() {
            CRT crt;
            long final_modulus = 1;
            for (const auto &entry : entries) {
                crt.add_entry(entry);
                final_modulus *= entry.modulus;
            }
            auto result = crt.solve();
            using namespace unit_test::checks;
            check_equal(result.remainder, expected, "result");
            check_equal(result.modulus, final_modulus, "modulus");
        });
    };

    suite.test("", []() {
        CRT crt;
        crt.add_entry(2, 5);
        crt.add_entry(3, 7);
        auto result = crt.solve();
        using namespace unit_test::checks;
        check_equal(result.remainder, 17, "result");
        check_equal(result.modulus, 35, "modulus");
    });

    test("", {{2, 5}, {3, 7}}, 17);
    test("original formulation", {{2, 3}, {3, 5}, {2, 7}}, 23);
    test("wikipedia example", {{0, 3}, {3, 4}, {4, 5}}, 39);
    test("single entry", {{1, 10}}, 1);

    return suite.done(), suite.num_failed();
}

} // namespace aoc::math::test

int main() {
    std::size_t failed_count = 0;
    using namespace aoc::math::test;
    failed_count += test_powers_of_10<int>();
    failed_count += test_powers_of_10<unsigned int>();
    failed_count += test_powers_of_10<std::int64_t>();
    failed_count += test_powers_of_10<std::uint64_t>();

    failed_count += test_num_digits<int>();
    failed_count += test_num_digits<unsigned int>();
    failed_count += test_num_digits<std::int64_t>();
    failed_count += test_num_digits<std::uint64_t>();

    failed_count += test_extended_gcd();
    failed_count += test_crt();

    return unit_test::fix_exit_code(failed_count);
}
