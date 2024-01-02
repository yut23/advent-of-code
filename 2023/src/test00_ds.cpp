/******************************************************************************
 * File:        test00_ds.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-01-01
 * Description: Unit tests for data structures.
 *****************************************************************************/

#include "unit_test/unit_test.hpp"

#include "data_structures.hpp"
#include <cstddef>    // for size_t
#include <functional> // for greater
#include <string>     // for string, to_string, string_literals

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
        assert(heap.size() == 2);
        heap.update(nh, 5);
        DO_TEST(heap.top() == 5);
        assert(heap.size() == 2);
        heap.update(nh, 10);
        assert(heap.size() == 2);
    }
    DO_TEST(heap.top() == 7);
    heap.pop();
    DO_TEST(heap.size() == 1);
    DO_TEST(heap.top() == 10);
    heap.pop();
    DO_TEST(heap.empty());
    return test.done(), test.num_failed();
}

} // namespace aoc::ds::test

int main() {
    std::size_t failed_count = 0;
    failed_count += aoc::ds::test::test_pairing_heap_max();
    failed_count += aoc::ds::test::test_pairing_heap_min();
    return unit_test::fix_exit_code(failed_count);
}
