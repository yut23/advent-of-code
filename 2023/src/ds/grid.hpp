#ifndef GRID_HPP_EF9V6ZF8
#define GRID_HPP_EF9V6ZF8

#include "lib.hpp"           // for Pos
#include "util/concepts.hpp" // for any_convertible_range

#include <algorithm> // for copy, move
#include <cassert>   // for assert
#include <compare>   // for partial_ordering
#include <concepts>  // for same_as, convertible_to
#include <cstddef>   // for size_t, ptrdiff_t
#include <iterator>  // for begin, end, back_inserter, misc concepts
#include <ranges>    // for range, range_value_t
#include <span>      // for span, dynamic_extent // IWYU pragma: export
#include <string>    // for string, basic_string
#include <type_traits> // for is_same_v, conditional_t, is_rvalue_reference_v // IWYU pragma: export
#include <utility> // for move, pair, make_pair
#include <vector>  // for vector, __cpp_lib_constexpr_vector

namespace aoc::ds {

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

#endif /* end of include guard: GRID_HPP_EF9V6ZF8 */
