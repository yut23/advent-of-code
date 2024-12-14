/******************************************************************************
 * File:        test_pretty_print.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-11
 * Description: Tests for the unit test pretty-printing code.
 *****************************************************************************/

#include "unit_test/pretty_print.hpp" // IWYU pragma: associated

#include "unit_test/unit_test.hpp"

#include <sstream> // for ostringstream

namespace pretty_print::test {

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
    failed_count += pretty_print::test::test_char_escaping();
    return unit_test::fix_exit_code(failed_count);
}
