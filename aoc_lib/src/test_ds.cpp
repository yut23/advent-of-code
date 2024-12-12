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
#include "unit_test/unit_test.hpp"    // for ManualTest, fix_exit_code
#include "util/util.hpp"              // for demangle

#include <array>      // for array
#include <cstddef>    // for size_t
#include <functional> // for greater
#include <iostream>   // for cerr
#include <string>     // for string
#include <typeinfo>   // for type_info

namespace aoc::ds::test {

#define DO_TEST(x)                                                             \
    do {                                                                       \
        test((x), #x);                                                         \
    } while (0)

std::size_t test_pairing_heap_max() {
    unit_test::ManualTest test("aoc::ds::pairing_heap<int>");
    pairing_heap<int> heap;
    int x = 1;
    heap.push(x);
    heap.push(1);
    {
        auto nh = heap.push(2);
        heap.emplace(3);
        DO_TEST(heap.top() == 3);
        heap.update(nh, 4);
    }
    DO_TEST(heap.top() == 4);
    heap.pop();
    DO_TEST(heap.top() == 3);
    DO_TEST(heap.size() == 3);
    heap.pop();
    DO_TEST(heap.top() == 1);
    heap.pop();
    DO_TEST(heap.top() == 1);
    heap.pop();
    DO_TEST(heap.size() == 0);
    DO_TEST(heap.empty());
    return test.done(), test.num_failed();
}

std::size_t test_pairing_heap_min() {
    unit_test::ManualTest test("aoc::ds::pairing_heap<int, std::greater>");
    pairing_heap<int, std::greater<int>> heap;
    int x = 5;
    heap.push(x);
    heap.push(3);
    {
        auto nh = heap.push(10);
        heap.emplace(7);
        DO_TEST(heap.top() == 3);
        heap.update(nh, 1);
    }
    DO_TEST(heap.top() == 1);
    heap.pop();
    DO_TEST(heap.top() == 3);
    DO_TEST(heap.size() == 3);
    heap.pop();
    DO_TEST(heap.top() == 5);
    heap.pop();
    {
        auto nh = heap.push(4);
        DO_TEST(heap.top() == 4);
        heap.update(nh, 5);
        DO_TEST(heap.top() == 5);
        heap.update(nh, 10);
    }
    DO_TEST(heap.top() == 7);
    heap.pop();
    DO_TEST(heap.size() == 1);
    DO_TEST(heap.top() == 10);
    heap.pop();
    DO_TEST(heap.empty());
    return test.done(), test.num_failed();
}

template <class T>
std::size_t test_grid() {
    const std::string type_name = util::demangle(typeid(T).name());
    unit_test::ManualTest test("aoc::ds::Grid<" + type_name + ">");
    std::array<T, 50> arr;
    {
        int i = 0;
        for (auto &x : arr) {
            x = i++;
        }
    }
    Grid<T> grid(5, 10, arr);
    DO_TEST(grid.in_bounds(4, 9));
    DO_TEST(!grid.in_bounds(5, 3));
    DO_TEST(!grid.in_bounds(0, 10));
    DO_TEST(!grid.in_bounds(-2, 4));
    test(
        [&grid]() {
            using pretty_print::repr;
            auto col_it = grid.begin();
            for (int y = 0; y < grid.height; ++y, ++col_it) {
                assert(col_it >= grid.begin());
                assert(col_it == grid.begin() + y);
                auto row_it = col_it->begin();
                for (int x = 0; x < grid.width; ++x, ++row_it) {
                    if (grid.at(x, y) != *row_it) {
                        std::cerr
                            << "mismatch at " << x << ", " << y << ": expected "
                            << pretty_print::repr(grid.at(x, y), true)
                            << ", got " << pretty_print::repr(*row_it, true)
                            << "\n";
                        return false;
                    }
                }
                if (row_it != col_it->end()) {
                    std::cerr << "iterator for row " << y << " not at end\n";
                    return false;
                }
            }
            if (col_it != grid.end()) {
                std::cerr << "grid iterator not at end\n";
                return false;
            }
            return true;
        },
        "input iterator");
    test(
        [&grid]() {
            using pretty_print::repr;
            auto col_it = grid.cbegin();
            for (int y = 0; y < grid.height; ++y, ++col_it) {
                assert(col_it >= grid.begin());
                assert(col_it == grid.begin() + y);
                assert(col_it >= grid.cbegin());
                assert(col_it == grid.cbegin() + y);
                auto row_it = col_it->begin();
                for (int x = 0; x < grid.width; ++x, ++row_it) {
                    if (grid.at(x, y) != *row_it) {
                        std::cerr
                            << "mismatch at " << x << ", " << y << ": expected "
                            << pretty_print::repr(grid.at(x, y), true)
                            << ", got " << pretty_print::repr(*row_it, true)
                            << "\n";
                        return false;
                    }
                }
                if (row_it != col_it->end()) {
                    std::cerr << "iterator for row " << y << " not at end\n";
                    return false;
                }
            }
            if (col_it != grid.cend()) {
                std::cerr << "grid iterator not at end\n";
                return false;
            }
            return true;
        },
        "input const_iterator");
    test(
        [&grid]() {
            using pretty_print::repr;
            auto col_it = grid.begin();
            for (int y = 0; y < grid.height; ++y, ++col_it) {
                assert(col_it >= grid.begin());
                assert(col_it == grid.begin() + y);
                auto row_it = col_it->begin();
                for (int x = 0; x < grid.width; ++x, ++row_it) {
                    auto new_val = 1 - *row_it;
                    *row_it = new_val;
                    if (grid.at(x, y) != new_val) {
                        std::cerr << "writing failed at " << x << ", " << y
                                  << "\n";
                        return false;
                    }
                }
                if (row_it != col_it->end()) {
                    std::cerr << "iterator for row " << y << " not at end\n";
                    return false;
                }
            }
            if (col_it != grid.end()) {
                std::cerr << "grid iterator not at end\n";
                return false;
            }
            return true;
        },
        "output iterator");

    return test.done(), test.num_failed();
}

} // namespace aoc::ds::test

int main() {
    std::size_t failed_count = 0;
    failed_count += aoc::ds::test::test_pairing_heap_max();
    failed_count += aoc::ds::test::test_pairing_heap_min();
    failed_count += aoc::ds::test::test_grid<int>();
    failed_count += aoc::ds::test::test_grid<bool>();
    return unit_test::fix_exit_code(failed_count);
}
