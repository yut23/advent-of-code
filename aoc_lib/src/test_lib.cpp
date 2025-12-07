/******************************************************************************
 * File:        test_lib.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-19
 * Description: Unit tests for library functions.
 *****************************************************************************/

#include "unit_test/unit_test.hpp"
#include "util/util.hpp" // for demangle

#include "lib.hpp"  // for expect_input, as_number
#include <cstddef>  // for size_t
#include <iostream> // for istream
#include <limits>   // for numeric_limits
#include <sstream>  // for istringstream, ostringstream, skipws, noskipws
#include <string>   // for string, to_string, string_literals
#include <typeinfo> // for type_info
// IWYU pragma: no_include <utility>  // for move (expect_input)

namespace aoc::test {

std::size_t test_expect_input() {
    using namespace std::string_literals;
    using namespace unit_test::checks;
    unit_test::TestSuite suite("aoc::expect_input");

    suite.test("skipws", []() {
        std::istringstream ss{"foobar baz {a=1, bc=32}"};
        std::string baz{"baz"};
        char close_brace = '}';
        ss >> std::skipws;
        check(ss >> aoc::expect_input('f'), "reading 'f' from stream");
        check(ss >> aoc::expect_input("oo"), "reading \"oo\" from stream");
        check(ss >> aoc::expect_input(std::string{"bar"}),
              "reading std::string{\"bar\"} from stream");
        check(ss >> aoc::expect_input(baz),
              "reading \"baz\"s as lvalue from stream");
        check(ss >> aoc::expect_input('{'), "reading '{' from stream");
        check(ss >> aoc::expect_input("a="s), "reading \"a=\"s from stream");
        check(ss >> aoc::expect_input(1), "reading 1 from stream");
        check(ss >> aoc::expect_input(","), "reading \",\" from stream");
        check(ss >> aoc::expect_input("b"s + "c"),
              "reading \"b\"s + \"c\" from stream");
        check(ss >> aoc::expect_input("="s), "reading \"=\"s from stream");
        check(ss >> aoc::expect_input('3') >> aoc::expect_input(2),
              "reading '3' and 2 from stream");
        check(ss >> aoc::expect_input(close_brace),
              "reading '}' as lvalue from stream");
    });
    return suite.done(), suite.num_failed();

    suite.test("noskipws", []() {
        std::istringstream ss{"foobar  baz {a=1,bc=32}"};
        std::string baz{" baz "};
        char close_brace = '}';
        ss >> std::noskipws;
        check(ss >> aoc::expect_input('f'), "reading 'f' from stream");
        check(ss >> aoc::expect_input("oo"), "reading \"oo\" from stream");
        check(ss >> aoc::expect_input(std::string{"bar "}),
              "reading std::string{\"bar\"} from stream");
        check(ss >> aoc::expect_input(baz),
              "reading \" baz \"s as lvalue from stream");
        check(ss >> aoc::expect_input('{'), "reading '{' from stream");
        check(ss >> aoc::expect_input("a="s), "reading \"a=\"s from stream");
        check(ss >> aoc::expect_input(1), "reading 1 from stream");
        check(ss >> aoc::expect_input(","), "reading \",\" from stream");
        check(ss >> aoc::expect_input("b"s + "c"),
              "reading \"b\"s + \"c\" from stream");
        check(ss >> aoc::expect_input("="s), "reading \"=\"s from stream");
        check(ss >> aoc::expect_input('3') >> aoc::expect_input(2),
              "reading '3' and 2 from stream");
        check(ss >> aoc::expect_input(close_brace),
              "reading '}' as lvalue from stream");
    });
    return suite.done(), suite.num_failed();
}

template <class CharT>
std::size_t test_as_number_extraction() {
    const std::string type_name = util::demangle(typeid(CharT).name());
    unit_test::PureTest test(
        "aoc::as_number " + type_name + " extraction",
        +[](const std::string &s) -> CharT {
            std::istringstream ss{s};
            CharT result;
            ss >> aoc::as_number(result);
            return result;
        });
    for (int i = 0; i <= std::numeric_limits<CharT>::max(); ++i) {
        test.run_on_args(std::to_string(i), static_cast<CharT>(i));
    }
    return test.done(), test.num_failed();
}

template <class CharT>
std::size_t test_as_number_insertion() {
    const std::string type_name = util::demangle(typeid(CharT).name());
    unit_test::PureTest test(
        "aoc::as_number " + type_name + " insertion",
        +[](CharT ch) -> std::string {
            std::ostringstream oss{};
            oss << aoc::as_number(ch);
            return oss.str();
        });
    for (int i = 0; i <= std::numeric_limits<CharT>::max(); ++i) {
        test.run_on_args(static_cast<CharT>(i), std::to_string(i));
    }
    return test.done(), test.num_failed();
}

} // namespace aoc::test

int main() {
    std::size_t failed_count = 0;
    failed_count += aoc::test::test_expect_input();
    failed_count += aoc::test::test_as_number_extraction<char>();
    failed_count += aoc::test::test_as_number_extraction<unsigned char>();
    failed_count += aoc::test::test_as_number_insertion<char>();
    failed_count += aoc::test::test_as_number_insertion<unsigned char>();
    return unit_test::fix_exit_code(failed_count);
}
