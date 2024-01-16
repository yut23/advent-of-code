/******************************************************************************
 * File:        data_structures.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-01-01
 * Description: Generic data structures not present in the STL.
 *****************************************************************************/

#ifndef DATA_STRUCTURES_HPP_KBRH53YC
#define DATA_STRUCTURES_HPP_KBRH53YC

#include "lib.hpp"          // for DEBUG
#include <cassert>          // for assert
#include <cstddef>          // for size_t
#include <functional>       // for less, greater
#include <initializer_list> // for initializer_list
#include <iterator>         // for next, advance
#include <list>             // for list
#include <memory> // for shared_ptr, weak_ptr, make_shared, enable_shared_from_this
#include <string>      // for string, string_literals
#include <type_traits> // for is_invocable_r_v // IWYU pragma: keep
#include <utility>     // for move, swap, forward, pair
#include <vector>      // for vector
// IWYU pragma: no_include <algorithm>  // for copy, fill_n

namespace aoc::ds {

template <class T, class Compare = std::less<T>>
    requires std::is_invocable_r_v<bool, Compare, T, T>
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
                 const value_compare &compare = value_compare());

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

/**
 * Two-dimensional grid of values, indexable by aoc::Pos.
 */
template <class T>
struct Grid {
    using value_type = T;
    using size_type = int;

    const size_type height;
    const size_type width;

  protected:
    using container_type = std::vector<value_type>;
    container_type data;

  public:
    using reference = typename container_type::reference;
    using const_reference = typename container_type::const_reference;

    using iterator = typename container_type::iterator;
    using const_iterator = typename container_type::const_iterator;

    constexpr inline std::size_t get_index(size_type x, size_type y) const {
        return y * width + x;
    }

    constexpr Grid(size_type width, size_type height,
                   const value_type &value = value_type())
        : height(height), width(width), data(height * width, value) {}
    constexpr Grid(size_type width, size_type height,
                   const std::vector<value_type> &data)
        : height(height), width(width), data(data) {
        assert(data.size() == height * width);
    }
    constexpr Grid(size_type width, size_type height,
                   std::vector<value_type> &&data)
        : height(height), width(width), data(std::move(data)) {
        assert(data.size() == height * width);
    }
    explicit Grid(const std::vector<std::vector<T>> &grid)
        : height(grid.size()), width(grid[0].size()), data(height * width) {
        assert(!grid.empty());
        for (int y = 0; y < height; ++y) {
            const std::vector<T> &row = grid[y];
            assert(static_cast<size_type>(row.size()) == width);
            for (int x = 0; x < width; ++x) {
                data[get_index(x, y)] = row[x];
            }
        }
    }

    constexpr Grid(const Grid &other) = default;
    constexpr Grid(Grid &&other) noexcept = default;

    constexpr Grid &operator=(const Grid &other) = default;
    constexpr Grid &operator=(Grid &&other) noexcept = default;

    constexpr iterator begin() noexcept { return data.begin(); }
    constexpr const_iterator begin() const noexcept { return data.begin(); }
    constexpr const_iterator cbegin() const noexcept { return data.cbegin(); }

    constexpr iterator end() noexcept { return data.end(); }
    constexpr const_iterator end() const noexcept { return data.end(); }
    constexpr const_iterator cend() const noexcept { return data.cend(); }

    constexpr bool in_bounds(size_type x, size_type y) const noexcept {
        return y >= 0 && x >= 0 && y < height && x < width;
    }
    constexpr bool in_bounds(const Pos &pos) const noexcept {
        return in_bounds(pos.x, pos.y);
    }

    constexpr reference at(size_type x, size_type y) {
        return data.at(get_index(x, y));
    }
    constexpr reference at(const Pos &pos) { return at(pos.x, pos.y); }

    constexpr const_reference at(size_type x, size_type y) const {
        return data.at(get_index(x, y));
    }
    constexpr const_reference at(const Pos &pos) const {
        return at(pos.x, pos.y);
    }

    constexpr reference operator[](const Pos &pos) {
        return data[get_index(pos.x, pos.y)];
    }
    constexpr const_reference operator[](const Pos &pos) const {
        return data[get_index(pos.x, pos.y)];
    }
};

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

constexpr bool _grid_lint_helper_constexpr() {
    Grid<int> grid1(5, 10, 0);
    Grid<std::pair<int, bool>> grid2(3, 2);
    Pos pos(2, 1);
    grid2[pos] = {1, true};
    return grid1.width == 5 && grid1.height == 10 && grid2.width == 3 &&
           grid2.height == 2 && grid2.at(2, 1).first == 1 &&
           grid2.at(2, 1).second == true;
}
[[maybe_unused]] void _grid_lint_helper() {
    Grid<int> grid1a(10, 5);
    Grid grid1b(10, 5, 10);
    static_assert(std::is_same_v<decltype(grid1a), decltype(grid1b)>);

    // cppcheck-suppress unreadVariable
    Grid<std::pair<int, std::string>> grid2a(3, 2);
    Grid grid2b(3, 2, std::make_pair(1, std::string{"a"}));
    static_assert(std::is_same_v<decltype(grid2a), decltype(grid2b)>);

    std::vector<std::vector<bool>> bool_vec{{true, false}, {true, false}};
    // cppcheck-suppress unreadVariable
    Grid<bool> grid3a(bool_vec);
    Grid grid3b(bool_vec);
    Grid grid3c(std::move(bool_vec));
    static_assert(std::is_same_v<decltype(grid3a), decltype(grid3b)>);
    static_assert(std::is_same_v<decltype(grid3a), decltype(grid3c)>);

    static_assert(_grid_lint_helper_constexpr());
}
} // namespace

} // namespace aoc::ds

#endif /* end of include guard: DATA_STRUCTURES_HPP_KBRH53YC */
