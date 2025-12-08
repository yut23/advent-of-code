#ifndef BOUNDED_HEAP_HPP_UWDOHMCV
#define BOUNDED_HEAP_HPP_UWDOHMCV

#include <algorithm>  // for push_heap, pop_heap
#include <cassert>    // for assert
#include <functional> // for less
#include <optional>   // for optional
#include <utility>    // for move, forward, swap
#include <vector>     // for vector

namespace aoc::ds {

template <class T, class Container = std::vector<T>,
          class Compare = std::less<typename Container::value_type>>
class bounded_heap {
  public:
    using container_type = Container;
    using value_compare = Compare;
    using value_type = typename Container::value_type;
    using size_type = typename Container::size_type;
    using reference = typename Container::reference;
    using const_reference = typename Container::const_reference;
    using const_iterator = typename Container::const_iterator;

    explicit bounded_heap(size_type max_size)
        : bounded_heap(max_size, Compare()) {}
    bounded_heap(size_type max_size, const Compare &compare)
        : c(), comp(compare), max_size(max_size) {}
    bounded_heap(const bounded_heap &) = default;
    bounded_heap(bounded_heap &&) = default;

    bounded_heap &operator=(const bounded_heap &other) {
        c = other.c;
        comp = other.comp;
        max_size = other.max_size;
        return *this;
    }
    bounded_heap &operator=(bounded_heap &&other) {
        c = std::move(other).c;
        comp = std::move(other).comp;
        max_size = std::move(other).max_size;
        return *this;
    }

    // Element access
    const_reference top() const { return c.front(); }
    const_iterator begin() const { return c.begin(); }
    const_iterator end() const { return c.end(); }

    // Capacity
    bool empty() const { return c.empty(); }
    size_type size() const { return c.size(); }

    // Modifiers
    void push(const value_type &value) {
        if (should_insert(value)) {
            c.push_back(value);
            std::push_heap(c.begin(), c.end(), comp);
        }
    }
    void push(value_type &&value) {
        if (should_insert(value)) {
            c.push_back(std::move(value));
            std::push_heap(c.begin(), c.end(), comp);
        }
    }

    void pop() {
        std::pop_heap(c.begin(), c.end(), comp);
        c.pop_back();
    }

    void swap(bounded_heap &other) noexcept(
        std::is_nothrow_swappable_v<Container> &&std::is_nothrow_swappable_v<
            Compare> &&std::is_nothrow_swappable_v<std::optional<value_type>>) {
        using std::swap;
        swap(c, other.c);
        swap(comp, other.comp);
        swap(max_size, other.max_size);
    }

  protected:
    bool should_insert(const value_type &value) {
        if (c.size() < max_size) {
            return true;
        }
        if (!comp(top(), value)) {
            pop();
            return true;
        }
        return false;
    }

    Container c;
    Compare comp;
    size_type max_size;
};

} // namespace aoc::ds

namespace std {
template <class T, class Container, class Compare, class Alloc>
struct uses_allocator<aoc::ds::bounded_heap<T, Compare, Container>, Alloc>
    : std::uses_allocator<Container, Alloc>::type {};

template <class T, class Container, class Compare>
void swap(aoc::ds::bounded_heap<T, Container, Compare> &lhs,
          aoc::ds::bounded_heap<T, Container, Compare>
              &rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}
} // namespace std

#endif /* end of include guard: BOUNDED_HEAP_HPP_UWDOHMCV */
