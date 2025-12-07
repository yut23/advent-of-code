/******************************************************************************
 * File:        test_ds.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-01-01
 * Description: Unit tests for data structures.
 *****************************************************************************/

#include "ds/grid.hpp"         // IWYU pragma: associated
#include "ds/pairing_heap.hpp" // IWYU pragma: associated

#include "unit_test/pretty_print.hpp" // for repr
#include "unit_test/unit_test.hpp"    // for TestSuite, fix_exit_code
#include "util/util.hpp"              // for demangle

#include <array>      // for array
#include <cstddef>    // for size_t
#include <functional> // for greater
#include <sstream>    // for stringstream
#include <string>     // for string
#include <typeinfo>   // for type_info
#include <vector>     // for vector

namespace aoc::ds::test {

#define HEAP_CHECK(op, value)                                                  \
    do {                                                                       \
        check_equal(heap.op(), value, "heap " #op " incorrect");               \
    } while (0)

std::size_t test_pairing_heap_max() {
    unit_test::TestSuite suite("aoc::ds::pairing_heap<int>");
    using namespace unit_test::checks;
    suite.test("", []() {
        pairing_heap<int> heap;
        int x = 1;
        heap.push(x);
        heap.push(1);
        {
            auto nh = heap.push(2);
            heap.emplace(3);
            HEAP_CHECK(top, 3);
            heap.update(nh, 4);
        }
        HEAP_CHECK(top, 4);
        heap.pop();
        HEAP_CHECK(top, 3);
        HEAP_CHECK(size, 3ul);
        heap.pop();
        HEAP_CHECK(top, 1);
        heap.pop();
        HEAP_CHECK(top, 1);
        heap.pop();
        HEAP_CHECK(size, 0ul);
        check(heap.empty(), "heap not empty");
    });
    return suite.done(), suite.num_failed();
}

std::size_t test_pairing_heap_min() {
    unit_test::TestSuite suite("aoc::ds::pairing_heap<int, std::greater>");
    using namespace unit_test::checks;
    suite.test("", []() {
        pairing_heap<int, std::greater<int>> heap;
        int x = 5;
        heap.push(x);
        heap.push(3);
        {
            auto nh = heap.push(10);
            heap.emplace(7);
            HEAP_CHECK(top, 3);
            heap.update(nh, 1);
        }
        HEAP_CHECK(top, 1);
        heap.pop();
        HEAP_CHECK(top, 3);
        HEAP_CHECK(size, 3ul);
        heap.pop();
        HEAP_CHECK(top, 5);
        heap.pop();
        {
            auto nh = heap.push(4);
            HEAP_CHECK(top, 4);
            heap.update(nh, 5);
            HEAP_CHECK(top, 5);
            heap.update(nh, 10);
        }
        HEAP_CHECK(top, 7);
        heap.pop();
        HEAP_CHECK(size, 1ul);
        HEAP_CHECK(top, 10);
        heap.pop();
        check(heap.empty(), "heap not empty");
    });
    return suite.done(), suite.num_failed();
}

#undef HEAP_CHECK

template <class T>
void test_grid_basic(unit_test::TestSuite &suite, const Grid<T> &grid) {
    using namespace unit_test::checks;
    suite.test("in_bounds", [&grid]() {
        check(grid.in_bounds(4, 9));
        check(!grid.in_bounds(5, 3));
        check(!grid.in_bounds(0, 10));
        check(!grid.in_bounds(-2, 4));
    });
}

template <class T>
void test_grid_iterators(unit_test::TestSuite &suite, Grid<T> grid) {
    using namespace unit_test::checks;
    suite.test("input iterator", [&grid]() {
        auto row_it = grid.begin();
        for (int y = 0; y < grid.height; ++y, ++row_it) {
            check(row_it >= grid.begin());
            check(row_it == grid.begin() + y);
            auto col_it = row_it->begin();
            for (int x = 0; x < grid.width; ++x, ++col_it) {
                check_equal(grid.at(x, y), *col_it, [&](auto &os) {
                    os << "mismatch at " << x << ", " << y;
                });
            }
            check_equal(col_it, row_it->end(), [&](auto &os) {
                os << "iterator for row " << y << " not at end";
            });
        }
        check_equal(row_it, grid.end(), "grid iterator not at end");
    });
    suite.test("input const_iterator", [&grid]() {
        auto row_it = grid.cbegin();
        for (int y = 0; y < grid.height; ++y, ++row_it) {
            check(row_it >= grid.begin());
            check(row_it == grid.begin() + y);
            check(row_it >= grid.cbegin());
            check(row_it == grid.cbegin() + y);
            auto col_it = row_it->begin();
            for (int x = 0; x < grid.width; ++x, ++col_it) {
                check_equal(grid.at(x, y), *col_it, [=](auto &os) {
                    os << "mismatch at " << x << ", " << y;
                });
            }
            check_equal(col_it, row_it->end(), [=](auto &os) {
                os << "iterator for row " << y << " not at end";
            });
        }
        check_equal(row_it, grid.cend(), "grid iterator not at end");
    });
    suite.test("output iterator", [&grid]() {
        auto row_it = grid.begin();
        for (int y = 0; y < grid.height; ++y, ++row_it) {
            check(row_it >= grid.begin());
            check(row_it == grid.begin() + y);
            auto col_it = row_it->begin();
            for (int x = 0; x < grid.width; ++x, ++col_it) {
                T new_val = 1 - *col_it;
                *col_it = new_val;
                check_equal(grid.at(x, y), new_val, [=](auto &os) {
                    os << "writing failed at " << x << ", " << y;
                });
            }
            check_equal(col_it, row_it->end(), [=](auto &os) {
                os << "iterator for row " << y << " not at end";
            });
        }
        check_equal(row_it, grid.end(), "grid iterator not at end");
    });
    suite.test("end iterator", [&grid]() {
        auto end_it = grid.end();
        for (int i = grid.height; i > 0; --i, --end_it) {
            check_equal(end_it, grid.begin() + i, [=](auto &os) {
                os << "grid.end() - " << grid.height - i
                   << " != grid.begin() + " << i;
            });
        }
        check_equal(end_it, grid.begin(), [=](auto &os) {
            os << "grid.end() - " << grid.height << " != grid.begin()";
        });
    });
}

template <class T>
std::size_t test_grid() {
    const std::string type_name = util::demangle(typeid(T).name());
    unit_test::TestSuite suite("aoc::ds::Grid<" + type_name + ">");

    std::array<T, 50> arr;
    for (int i = 0; auto &x : arr) {
        x = i++;
    }
    Grid<T> grid(5, 10, arr);

    test_grid_basic(suite, grid);
    test_grid_iterators(suite, grid);
    return suite.done(), suite.num_failed();
}

std::size_t test_grid_repr() {
    unit_test::TestSuite suite("aoc::ds::Grid repr");

    std::vector<int> data(12, 0);
    for (int i = 0; auto &x : data) {
        x = i++;
    }
    Grid<int> grid(3, 4, std::move(data));

    using namespace unit_test::checks;
    suite.test("default width", [&grid]() {
        std::stringstream oss;
        oss << pretty_print::repr(grid);
        std::string expected = ("[[0 1 2],\n"
                                " [3 4 5],\n"
                                " [6 7 8],\n"
                                " [9 10 11]]");
        check_equal(oss.str(), expected);
    });
    suite.test("explicit width", [&grid]() {
        std::ostringstream oss;
        oss << std::setw(2);
        oss << pretty_print::repr(grid);
        std::string expected = ("[[ 0  1  2],\n"
                                " [ 3  4  5],\n"
                                " [ 6  7  8],\n"
                                " [ 9 10 11]]");
        check_equal(oss.str(), expected);
    });
    return suite.done(), suite.num_failed();
}

} // namespace aoc::ds::test

int main() {
    std::size_t failed_count = 0;
    failed_count += aoc::ds::test::test_pairing_heap_max();
    failed_count += aoc::ds::test::test_pairing_heap_min();
    failed_count += aoc::ds::test::test_grid<int>();
    failed_count += aoc::ds::test::test_grid<bool>();
    failed_count += aoc::ds::test::test_grid_repr();
    // run linter helper functions, to catch any failed asserts
    aoc::ds::test::_pairing_heap_lint_helper();
    aoc::ds::test::_grid_lint_helper();
    return unit_test::fix_exit_code(failed_count);
}
