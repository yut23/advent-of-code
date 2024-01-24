/******************************************************************************
 * File:        data_structures.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-01-01
 * Description: Generic data structures not present in the STL.
 *****************************************************************************/

#ifndef DATA_STRUCTURES_HPP_KBRH53YC
#define DATA_STRUCTURES_HPP_KBRH53YC

#include "lib.hpp"           // for Pos, DEBUG
#include "util/concepts.hpp" // for any_convertible_range

#include <algorithm>        // for copy
#include <cassert>          // for assert
#include <compare>          // for partial_ordering
#include <concepts>         // for predicate, same_as, convertible_to
#include <cstddef>          // for size_t, ptrdiff_t
#include <functional>       // for less, greater
#include <initializer_list> // for initializer_list
#include <iterator>         // for next, advance, back_inserter, misc concepts
#include <list>             // for list
#include <memory> // for shared_ptr, weak_ptr, make_shared, enable_shared_from_this
#include <ranges>      // for range, range_value_t
#include <span>        // for span, dynamic_extent // IWYU pragma: export
#include <string>      // for string, basic_string
#include <type_traits> // for is_same_v, conditional_t // IWYU pragma: export
#include <utility>     // for move, swap, forward, pair, make_pair
#include <vector>      // for vector, __cpp_lib_constexpr_vector

namespace aoc::ds {

template <class T, std::predicate<T, T> Compare = std::less<T>>
class pairing_heap {
  public:
    using value_compare = Compare;
    using value_type = T;
    using size_type = std::size_t;
    using reference = value_type &;
    using const_reference = const value_type &;

    class pair_node : public std::enable_shared_from_this<pair_node> {
      private:
        value_type element;
        std::shared_ptr<pair_node> first_child{};
        std::shared_ptr<pair_node> next_sibling{};
        std::weak_ptr<pair_node> prev{};

        void check_structure() const {
            if constexpr (!aoc::DEBUG) {
                return;
            }
            if (first_child) {
                std::shared_ptr<pair_node> node = first_child->prev.lock();
                // first_child->prev should point to this node
                assert(node);
                assert(node.get() == this);
                // recursively check first_child
                first_child->check_structure();
            }
            if (next_sibling) {
                // the root node should not have any siblings, so prev
                // should exist
                assert(prev.lock());
                // next_sibling->prev should point to this node
                std::shared_ptr<pair_node> node = next_sibling->prev.lock();
                assert(node);
                assert(node.get() == this);
                // recursively check next_sibling
                next_sibling->check_structure();
            }
        }

        /**
         * Insert a node as the leftmost child of this node.
         */
        void insert_left(std::shared_ptr<pair_node> node) {
            check_structure();
            node->check_structure();
            assert(!node->next_sibling);
            if (std::shared_ptr<pair_node> tmp = node->prev.lock()) {
                assert(!tmp);
            }
            std::shared_ptr<pair_node> old_first_child = first_child;
            node->next_sibling = old_first_child;
            if (old_first_child) {
                if (std::shared_ptr<pair_node> tmp =
                        old_first_child->prev.lock()) {
                    assert(tmp == get_ptr());
                }
                old_first_child->prev = node;
            }
            node->prev = get_weak_ptr();
            first_child = node;
        }

        /**
         * Extracts this node from the tree.
         */
        std::shared_ptr<pair_node> extract() {
            check_structure();
            // need to acquire a reference to this node, so it doesn't get
            // deleted out from under us
            std::shared_ptr<pair_node> ret = get_ptr();
            {
                std::shared_ptr<pair_node> previous = prev.lock();
                assert(previous);
                if (previous->first_child.get() == this) {
                    // we're the leftmost child of our parent
                    previous->first_child = next_sibling;
                } else {
                    previous->next_sibling = next_sibling;
                }
                if (next_sibling) {
                    next_sibling->prev = prev;
                    next_sibling.reset();
                }
                previous->check_structure();
            }
            prev.reset();
            check_structure();
            return ret;
        }

        static std::shared_ptr<pair_node> meld(std::shared_ptr<pair_node> node1,
                                               std::shared_ptr<pair_node> node2,
                                               const Compare &comp) {
            node1->check_structure();
            node2->check_structure();
            if (comp(node1->element, node2->element)) {
                std::swap(node1, node2);
            }
            node1->insert_left(node2);
            node1->check_structure();
            node2->check_structure();
            return node1;
        }

        // for enable_shared_from_this
        struct Private {};

      public:
        pair_node(Private, const value_type &element) : element(element) {}
        pair_node(Private, value_type &&element)
            : element(std::move(element)) {}
        template <class... Args>
        pair_node(Private, Args &&...args)
            : element(std::forward<Args>(args)...) {}

        const value_type &value() const { return element; }

        template <class... Args>
        static std::shared_ptr<pair_node> create(Args &&...args) {
            return std::make_shared<pair_node>(Private(),
                                               std::forward<Args>(args)...);
        }

        std::shared_ptr<pair_node> get_ptr() {
            return std::enable_shared_from_this<pair_node>::shared_from_this();
        }
        std::weak_ptr<pair_node> get_weak_ptr() {
            return std::enable_shared_from_this<pair_node>::weak_from_this();
        }

        friend class pairing_heap;
    };

    using handle_type = std::shared_ptr<pair_node>;

  private:
    value_compare _m_comp;
    size_type _m_size;
    std::shared_ptr<pair_node> root;

    void meld(std::shared_ptr<pair_node> other_root, size_type other_size) {
        if (root) {
            root = pair_node::meld(root, other_root, _m_comp);
        } else {
            root = other_root;
        }
        _m_size += other_size;
    }

  public:
    // default constructor
    pairing_heap() : pairing_heap(value_compare()) {}
    explicit pairing_heap(const value_compare &compare)
        : _m_comp(compare), _m_size(0), root() {}
    // copy constructor
    // pairing_heap(const pairing_heap &other);
    // move constructor
    // pairing_heap(pairing_heap &&other) = default;

    /// Constructs a heap with the contents of the range [`first`, `last`).
    template <class InputIt>
    pairing_heap(InputIt first, InputIt last,
                 const value_compare &compare = value_compare())
        : pairing_heap(compare) {
        for (; first != last; ++first) {
            emplace(*first);
        }
    }

    /// Constructs a heap from an initializer list.
    pairing_heap(std::initializer_list<value_type> init,
                 const value_compare &compare = value_compare())
        : pairing_heap(init.begin(), init.end(), compare) {}

    // copy assignment operator
    // pairing_heap &operator=(const pairing_heap &other);
    // move assignment operator
    // pairing_heap &operator=(pairing_heap &&other) = default;

    /// Returns a reference to the top element in the heap.
    const_reference top() const { return root->element; }

    /// Checks whether the heap is empty.
    [[nodiscard]] bool empty() const { return _m_size == 0; }

    /// Returns the number of elements in the heap.
    size_type size() const { return _m_size; }

    /**
     * Inserts an element into the heap.
     */
    handle_type push(const value_type &value) {
        auto node = pair_node::create(value);
        meld(node, 1);
        return node;
    }

    /**
     * Moves an element into the heap.
     */
    handle_type push(value_type &&value) {
        auto node = pair_node::create(std::move(value));
        meld(node, 1);
        return node;
    }

    /**
     * Constructs an element in-place and inserts it into the heap.
     */
    template <class... Args>
    handle_type emplace(Args &&...args) {
        auto node = pair_node::create(std::forward<Args>(args)...);
        meld(node, 1);
        return node;
    }

    /**
     * Removes the top element from the heap.
     */
    void pop() {
        // extract all the children of the root node
        assert(!root->next_sibling);
        std::list<std::shared_ptr<pair_node>> children;
        while (root->first_child) {
            children.push_back(root->first_child->extract());
        }
        root.reset();
        // left-to-right pass, meld consecutive pairs of child nodes
        for (auto it = children.begin();
             it != children.end() && std::next(it) != children.end();
             std::advance(it, 2)) {
            auto next_it = std::next(it);
            *it = pair_node::meld(*it, *next_it, _m_comp);
            children.erase(next_it);
        }
        // right-to-left pass, meld all the nodes together
        while (!children.empty()) {
            meld(children.back(), 0);
            children.pop_back();
        }
        --_m_size;
    }

    /**
     * Assigns v to the element handled by `handle` and updates the heap.
     */
    void update(handle_type handle, const value_type &value) {
        std::shared_ptr<pair_node> node;
        if (handle == root) {
            // to update the root node, we need to call pop instead of extract
            node = root;
            pop();
            // fix the size
            ++_m_size;
        } else {
            // remove the node from the tree
            node = handle->extract();
        }
        // update the element contained in the node
        node->element = value;
        meld(node, 0);
    }
};

// template deduction guide (based on std::vector)
template <class InputIt,
          class Compare =
              std::less<typename std::iterator_traits<InputIt>::value_type>>
pairing_heap(InputIt, InputIt, const Compare & = Compare())
    -> pairing_heap<typename std::iterator_traits<InputIt>::value_type,
                    Compare>;

/*
 * This is strictly a bidirectional iterator, although it supports all
 * operations needed for a random-access iterator aside from subscripting. It
 * is also an input iterator, since we don't want to allow writing to an entire
 * row.
 *
 * Based on
 * https://www.internalpointers.com/post/writing-custom-iterators-modern-cpp
 */
template <class T>
struct RowIterator {
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = std::span<T, std::dynamic_extent>;
    using pointer = const std::span<T> *;
    using reference = const std::span<T> &;

    RowIterator() = default;

    template <class R>
    explicit RowIterator(R &&range, std::size_t width)
        : m_span(range), m_width(width), m_index(0),
          m_curr_span(m_span.subspan(m_index * m_width, m_width)) {}

    // this is intentionally implicit to allow converting a non-const iterator
    // into a const one
    template <class U>
        requires std::same_as<T, const U>
    // cppcheck-suppress noExplicitConstructor
    RowIterator(const RowIterator<U> &other)
        : m_span(other.m_span), m_width(other.m_width), m_index(other.m_index),
          m_curr_span(other.m_curr_span) {}

    reference operator*() const { return m_curr_span; }
    pointer operator->() const { return &m_curr_span; }
    /*
     * This is all that's needed to be a full random-access iterator, but we
     * would have to store the subspan in order to return a reference.
     */
    // reference operator[](difference_type off) const {
    //     return m_span.subspan((m_index + off) * m_width, m_width);
    // }

    // Prefix increment
    RowIterator &operator++() {
        ++m_index;
        update_curr_span();
        return *this;
    }
    // Postfix increment
    RowIterator operator++(int) {
        RowIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    // Prefix decrement
    RowIterator &operator--() {
        --m_index;
        update_curr_span();
        return *this;
    }
    // Postfix decrement
    RowIterator operator--(int) {
        RowIterator tmp = *this;
        --(*this);
        return tmp;
    }

    // random-access iterator operations
    RowIterator &operator+=(difference_type n) {
        m_index += n;
        if (n != 0) {
            update_curr_span();
        }
        return *this;
    }
    RowIterator operator+(difference_type rhs) const {
        RowIterator tmp = *this;
        tmp += rhs;
        return tmp;
    }
    // pass-by-value makes a copy in rhs
    friend RowIterator operator+(difference_type lhs, RowIterator rhs) {
        rhs += lhs;
        return rhs;
    }

    RowIterator &operator-=(difference_type n) {
        m_index += -n;
        return *this;
    }
    RowIterator operator-(difference_type rhs) const {
        RowIterator tmp = *this;
        tmp += -rhs;
        return tmp;
    }

    difference_type operator-(const RowIterator &rhs) const {
        assert(this->m_span.data() == rhs.m_span.data());
        return this->m_index - rhs.m_index;
    }

    friend std::partial_ordering operator<=>(const RowIterator &lhs,
                                             const RowIterator &rhs) {
        if (lhs.m_curr_span.data() == rhs.m_curr_span.data()) {
            return std::partial_ordering::equivalent;
        }
        if (lhs.m_span.data() != rhs.m_span.data()) {
            // lhs and rhs belong to different Grids
            return std::partial_ordering::unordered;
        }
        return lhs.m_index <=> rhs.m_index;
    }

    // RowIterators that have been advanced past the end have an empty
    // m_curr_span (data() returns nullptr), which is also true of
    // default-constructed RowIterators, so RowIterator() is a valid sentinel.
    friend bool operator==(const RowIterator &a, const RowIterator &b) {
        return a.m_curr_span.data() == b.m_curr_span.data();
    }
    friend bool operator!=(const RowIterator &a, const RowIterator &b) {
        return a.m_curr_span.data() != b.m_curr_span.data();
    }

  private:
    // iterators must be default-constructible, so include defaults for all
    // members
    value_type m_span = {};
    std::size_t m_width = 0;
    std::size_t m_index = 0;
    value_type m_curr_span = {};

    void update_curr_span() {
        if (m_index * m_width + m_width <= m_span.size()) {
            m_curr_span = m_span.subspan(m_index * m_width, m_width);
        } else {
            m_curr_span = {};
        }
    }

    // required for const conversion
    friend struct RowIterator<const T>;
};

/**
 * Two-dimensional grid of values, indexable by aoc::Pos.
 */
template <class T>
struct Grid {
    using value_type = T;
    using size_type = int;

    using reference = value_type &;
    using const_reference = const value_type &;

    using iterator = RowIterator<value_type>;
    using const_iterator = RowIterator<const value_type>;

    // work around std::vector<bool> specialization
    // thanks to Dwayne Robinson on Stack Overflow for the basic_string idea:
    // <https://stackoverflow.com/a/75409768>
    using container_type =
        std::conditional_t<std::same_as<T, bool>, std::basic_string<value_type>,
                           std::vector<value_type>>;

    const size_type height;
    const size_type width;

  protected:
    container_type m_data;

  public:
    constexpr Grid(size_type width, size_type height,
                   const value_type &value = value_type())
        : height(height), width(width), m_data(height * width, value) {}
    // construct from a flat range
    template <util::concepts::any_convertible_range<value_type> R>
    constexpr Grid(size_type width, size_type height, R &&range)
        : height(height), width(width),
          m_data(std::begin(range), std::end(range)) {
        assert(static_cast<int>(m_data.size()) == height * width);
    }
    // move-construct from a flat data container
    constexpr Grid(size_type width, size_type height, container_type &&data)
        : height(height), width(width), m_data(std::move(data)) {
        assert(data.size() == height * width);
    }
    // construct from nested ranges
    template <std::ranges::range V>
        requires std::ranges::range<std::ranges::range_value_t<V>> &&
                     std::same_as<std::ranges::range_value_t<
                                      std::ranges::range_value_t<V>>,
                                  value_type>
    explicit Grid(V &&grid)
        : height(grid.size()), width(grid[0].size()), m_data() {
        assert(!grid.empty());
        m_data.reserve(height * width);
        auto data_it = std::back_inserter(m_data);
        for (auto &&row : grid) {
            assert(static_cast<size_type>(row.size()) == width);
            if constexpr (std::is_rvalue_reference_v<V>) {
                data_it = std::move(row.begin(), row.end(), data_it);
            } else {
                data_it = std::copy(row.begin(), row.end(), data_it);
            }
        }
    }

    constexpr Grid(const Grid &other) = default;
    constexpr Grid(Grid &&other) noexcept = default;

    constexpr Grid &operator=(const Grid &other) = default;
    constexpr Grid &operator=(Grid &&other) noexcept = default;

    constexpr inline std::size_t get_index(size_type x, size_type y) const {
        return y * width + x;
    }
    constexpr Pos index_to_pos(std::size_t index) const {
        return Pos(index % width, index / width);
    }

    constexpr iterator begin() noexcept { return iterator(m_data, width); }
    constexpr const_iterator begin() const noexcept {
        return const_iterator(m_data, width);
    }
    constexpr const_iterator cbegin() const noexcept {
        return const_iterator(m_data, width);
    }

    constexpr iterator end() noexcept { return iterator(); }
    constexpr const_iterator end() const noexcept { return const_iterator(); }
    constexpr const_iterator cend() const noexcept { return const_iterator(); }

    constexpr bool in_bounds(size_type x, size_type y) const noexcept {
        return y >= 0 && x >= 0 && y < height && x < width;
    }
    constexpr bool in_bounds(const Pos &pos) const noexcept {
        return in_bounds(pos.x, pos.y);
    }

    // indexing by x and y separately
    constexpr reference at(size_type x, size_type y) {
        return m_data.at(get_index(x, y));
    }
    constexpr const_reference at(size_type x, size_type y) const {
        return m_data.at(get_index(x, y));
    }

    // indexing by Pos
    constexpr reference at(const Pos &pos) { return at(pos.x, pos.y); }
    constexpr const_reference at(const Pos &pos) const {
        return at(pos.x, pos.y);
    }
    constexpr reference operator[](const Pos &pos) {
        return m_data[get_index(pos.x, pos.y)];
    }
    constexpr const_reference operator[](const Pos &pos) const {
        return m_data[get_index(pos.x, pos.y)];
    }

    // raw indexing into data
    constexpr reference at(std::size_t index) { return m_data.at(index); }
    constexpr const_reference at(std::size_t index) const {
        return m_data.at(index);
    }
    constexpr reference operator[](std::size_t index) { return m_data[index]; }
    constexpr const_reference operator[](std::size_t index) const {
        return m_data[index];
    }

    constexpr container_type &data() { return m_data; }
    constexpr const container_type &data() const { return m_data; }
};

template <class T>
Grid(const std::vector<std::vector<T>> &) -> Grid<T>;
template <class T>
Grid(std::vector<std::vector<T>> &&) -> Grid<T>;

// instantiate templates in an anonymous namespace, so static analyzers will
// check these functions
namespace {
[[maybe_unused]] void _pairing_heap_lint_helper() {
    // default constructor
    pairing_heap<int> h1a;
    // initializer_list constructor
    pairing_heap h1b({1, 2, 3, 4});
    pairing_heap h1c{1, 2, 3, 4};
    static_assert(std::is_same_v<decltype(h1a), decltype(h1b)>);
    static_assert(std::is_same_v<decltype(h1a), decltype(h1c)>);

    pairing_heap<int, std::greater<int>> h2;

    pairing_heap<std::pair<int, std::string>> h3a;
    // check template deduction guide
    std::vector<std::pair<int, std::string>> vec(1);
    pairing_heap h3b(vec.begin(), vec.end());
    static_assert(std::is_same_v<decltype(h3a), decltype(h3b)>);

    pairing_heap<std::pair<int, int *>, std::greater<std::pair<int, int *>>> h4;
}

#if __cpp_lib_constexpr_vector
constexpr bool _grid_lint_helper_constexpr() {
    Grid<int> grid1(5, 10, 0);
    Grid<std::pair<int, bool>> grid2(3, 2);
    Pos pos(2, 1);
    grid2[pos] = {1, true};
    return grid1.width == 5 && grid1.height == 10 && grid2.width == 3 &&
           grid2.height == 2 && grid2.at(2, 1).first == 1 &&
           grid2.at(2, 1).second == true;
}
#endif
[[maybe_unused]] void _grid_lint_helper() {
    Grid<int> grid1a(10, 5);
    Grid grid1b(10, 5, 10);
    static_assert(std::is_same_v<decltype(grid1a), decltype(grid1b)>);

    Grid<std::pair<int, std::string>> grid2a(3, 2);
    Grid grid2b(3, 2, std::make_pair(1, std::string{"a"}));
    static_assert(std::is_same_v<decltype(grid2a), decltype(grid2b)>);

    std::vector<std::vector<bool>> bool_vec{{true, false}, {true, false}};
    Grid<bool> grid3a(bool_vec);
    Grid grid3b(bool_vec);
    Grid grid3c(std::move(bool_vec));
    static_assert(std::is_same_v<decltype(grid3a), decltype(grid3b)>);
    static_assert(std::is_same_v<decltype(grid3a), decltype(grid3c)>);

    static_assert(std::bidirectional_iterator<Grid<int>::iterator>);
    static_assert(!std::random_access_iterator<Grid<int>::iterator>);
    static_assert(!std::contiguous_iterator<Grid<int>::iterator>);

    static_assert(std::bidirectional_iterator<Grid<bool>::iterator>);
    static_assert(!std::random_access_iterator<Grid<bool>::iterator>);
    static_assert(!std::contiguous_iterator<Grid<bool>::iterator>);

    {
        int i = 0;
        int y = 0;
        for (auto &row : grid1a) {
            int x = 0;
            for (auto &val : row) {
                val = i++;
                assert(grid1a.at(x, y) == i);
                ++x;
            }
            ++y;
        }
    }

    // an iterator should be assignable to a const_iterator
    static_assert(
        std::convertible_to<Grid<int>::iterator, Grid<int>::const_iterator>);
    static_assert(
        std::convertible_to<Grid<bool>::iterator, Grid<bool>::const_iterator>);

#if __cpp_lib_constexpr_vector
    static_assert(_grid_lint_helper_constexpr());
#endif
}
} // namespace

} // namespace aoc::ds

#endif /* end of include guard: DATA_STRUCTURES_HPP_KBRH53YC */
