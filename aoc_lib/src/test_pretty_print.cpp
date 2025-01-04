/******************************************************************************
 * File:        test_pretty_print.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-11
 * Description: Tests for the unit test pretty-printing code.
 *****************************************************************************/

#include "unit_test/pretty_print.hpp" // IWYU pragma: associated

#include "unit_test/unit_test.hpp"
#include "util/util.hpp" // for demangle

#include <source_location> // for source_location
#include <sstream>         // for ostringstream
#include <string>          // for string
#include <typeinfo>        // for type_info

namespace pretty_print::test {

std::size_t test_repr() {
    unit_test::TestSuite suite("pretty_print::repr");

    const auto test = [&suite](auto &&value, const std::string &expected,
                               bool result = false,
                               const std::source_location loc =
                                   std::source_location::current()) {
        const std::string type_name =
            util::demangle(typeid(std::remove_cvref_t<decltype(value)>).name());
        suite.test(type_name, [&value, &expected, &result, &loc]() {
            std::ostringstream oss{};
            oss << pretty_print::repr(value, result);
            unit_test::checks::check_equal(oss.str(), expected, "", loc);
        });
    };

    using namespace std::string_literals;
    // templated containers
    test(std::vector<int>{1, 2, 3}, "{1, 2, 3}");
    test(std::list<std::set<int>>{{1, 2, 3}, {2, 3, 4}, {3, 4, 5}},
         "{{1, 2, 3}, {2, 3, 4}, {3, 4, 5}}");
    test(std::array<float, 4>{1.5, 2.4, 3.2, -2.8}, "{1.5, 2.4, 3.2, -2.8}");
    test(std::map<int, std::string>{{0, "a"}, {2, "b"}, {100, "c"}},
         R"({0: "a", 2: "b", 100: "c"})");
    test(std::tuple<int, float, int>{1, 0.3, 4}, "[1, 0.3, 4]");
    test(std::optional<int>{15}, "15");
    test(std::optional<int>{}, "{}");

    // strings
    test("asdf"s, R"("asdf")");
    test("foo\r\nbar\" baz"s, R"("foo\r\nbar\" baz")");
    test("foo\r\nbar\" baz", R"("foo\r\nbar\" baz")");
    test(std::string_view{"string view"}, R"("string view")");

    // other leaf types
    test(std::strong_ordering::less, "less");
    test(std::strong_ordering::greater, "greater");
    test(std::strong_ordering::equal, "equal");
    test(std::strong_ordering::equivalent, "equal");

    test(true, "true");
    test(false, "false");

    test(1.4f, "1.4");
    test(3.14, "3.14");
    // includes hex format for results
    test(1.4f, "1.4 (0x1.666666p+0)", true);
    test(3.14, "3.14 (0x1.91eb851eb851fp+1)", true);

    return suite.done(), suite.num_failed();
}

std::size_t test_char_escaping() {
    unit_test::PureTest test(
        "pretty_print::repr char escaping", +[](char ch) -> std::string {
            std::ostringstream oss{};
            oss << pretty_print::repr(ch);
            return oss.str();
        });

    test('a', R"('a')");
    test(' ', R"(' ')");
    test('\'', R"('\'')");
    test('\"', R"('"')");
    test('\\', R"('\\')");
    test('\x00', R"('\x00')");
    test('\x01', R"('\x01')");
    test('\x02', R"('\x02')");
    test('\x03', R"('\x03')");
    test('\x04', R"('\x04')");
    test('\x05', R"('\x05')");
    test('\x06', R"('\x06')");
    test('\x07', R"('\x07')");
    test('\x08', R"('\x08')");
    test('\t', R"('\t')");
    test('\x09', R"('\t')");
    test('\n', R"('\n')");
    test('\x0a', R"('\n')");
    test('\x0b', R"('\x0b')");
    test('\x0c', R"('\x0c')");
    test('\r', R"('\r')");
    test('\x0d', R"('\r')");
    test('\x0e', R"('\x0e')");
    test('\x0f', R"('\x0f')");
    test('\x10', R"('\x10')");
    test('\x11', R"('\x11')");
    test('\x12', R"('\x12')");
    test('\x13', R"('\x13')");
    test('\x14', R"('\x14')");
    test('\x15', R"('\x15')");
    test('\x16', R"('\x16')");
    test('\x17', R"('\x17')");
    test('\x18', R"('\x18')");
    test('\x19', R"('\x19')");
    test('\x1a', R"('\x1a')");
    test('\x1b', R"('\x1b')");
    test('\033', R"('\x1b')");
    test('\x1c', R"('\x1c')");
    test('\x1d', R"('\x1d')");
    test('\x1e', R"('\x1e')");
    test('\x1f', R"('\x1f')");
    test('\x7f', R"('\x7f')");
    test('\x80', R"('\x80')");
    test('\xfe', R"('\xfe')");
    test('\xff', R"('\xff')");

    return test.done(), test.num_failed();
}

} // namespace pretty_print::test

int main() {
    std::size_t failed_count = 0;
    failed_count += pretty_print::test::test_repr();
    failed_count += pretty_print::test::test_char_escaping();
    return unit_test::fix_exit_code(failed_count);
}
