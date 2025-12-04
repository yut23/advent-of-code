#ifndef GRID_HPP_EF9V6ZF8
#define GRID_HPP_EF9V6ZF8

#include "lib.hpp" // for Pos
#include "unit_test/pretty_print.hpp"
#include "util/concepts.hpp" // for any_convertible_range

#include <algorithm> // for copy, move
#include <array>     // for array
#include <cassert>   // for assert
#include <compare>   // for partial_ordering
#include <concepts>  // for same_as, convertible_to
#include <cstdlib>   // for abs, size_t, ptrdiff_t
#include <iomanip>   // for setw
#include <iostream>  // for ostream
#include <iterator>  // for begin, end, back_inserter, misc concepts
#include <ranges>    // for range, range_value_t
#include <span>      // for span, dynamic_extent // IWYU pragma: export
#include <string>    // for string, basic_string
#include <type_traits> // for conditional_t, is_rvalue_reference_v // IWYU pragma: export
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

template <typename Func, typename T>
concept KernelFunc = std::invocable<Func, const T &, const Pos &, int> ||
                     std::invocable<Func, const T &, const Pos &> ||
                     std::invocable<Func, const T &>;

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
    using container_type = std::conditional_t<std::same_as<value_type, bool>,
                                              std::basic_string<value_type>,
                                              std::vector<value_type>>;

    const size_type height;
    const size_type width;

  protected:
    container_type m_data;

  public:
    // construct with the same value everywhere
    constexpr Grid(size_type width, size_type height,
                   const value_type &value = value_type())
        : height(height), width(width), m_data(height * width, value) {}
    // construct from a flat range (note: not compatible with CTAD)
    template <util::concepts::any_convertible_range<value_type> R>
    constexpr Grid(size_type width, size_type height, R &&range)
        : height(height), width(width),
          m_data(std::begin(range), std::end(range)) {
        assert(static_cast<size_type>(m_data.size()) == height * width);
    }
    // move-construct from a flat data container (note: not compatible with
    // CTAD)
    constexpr Grid(size_type width, size_type height, container_type &&data)
        : height(height), width(width), m_data(std::move(data)) {
        assert(static_cast<size_type>(m_data.size()) == height * width);
    }
    // construct from nested ranges
    template <std::ranges::range V>
        requires std::ranges::range<std::ranges::range_value_t<V>> &&
                     std::same_as<std::ranges::range_value_t<
                                      std::ranges::range_value_t<V>>,
                                  value_type> &&
                     (!std::same_as<std::remove_cvref_t<V>, Grid<value_type>>)
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

    // construct a grid with the same dimensions as another grid
    template <class U>
    Grid(const Grid<U> &other_grid, const value_type &value)
        : Grid(other_grid.width, other_grid.height, value) {}

    constexpr Grid(const Grid &other) = default;
    constexpr Grid(Grid &&other) noexcept = default;

    constexpr Grid &operator=(const Grid &other) = default;
    constexpr Grid &operator=(Grid &&other) noexcept = default;

    constexpr inline std::size_t get_index(size_type x, size_type y) const {
        return y * width + x;
    }
    constexpr inline std::size_t get_index(const Pos &pos) const {
        return get_index(pos.x, pos.y);
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
    constexpr reference at_unchecked(size_type x, size_type y) {
        return m_data[get_index(x, y)];
    }
    constexpr const_reference at_unchecked(size_type x, size_type y) const {
        return m_data[get_index(x, y)];
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

    /**
     * Calls func(value[, pos]) for each position in the grid.
     */
    template <typename Func>
        requires std::invocable<Func, const value_type &, const Pos &> ||
                 std::invocable<Func, const value_type &>
    constexpr void for_each(Func &&func) const {
        Pos p;
        for (p.y = 0; p.y < height; ++p.y) {
            for (p.x = 0; p.x < width; ++p.x) {
                if constexpr (std::invocable<Func, const value_type &,
                                             const Pos &>) {
                    func((*this)[p], p);
                } else {
                    func((*this)[p]);
                }
            }
        }
    }

    /**
     * Calls func(value[, pos]) for each position in the grid. Allows in-place
     * modification of the value.
     */
    template <typename Func>
        requires std::invocable<Func, value_type &, const Pos &> ||
                 std::invocable<Func, value_type &>
    constexpr void for_each(Func &&func) {
        Pos p;
        for (p.y = 0; p.y < height; ++p.y) {
            for (p.x = 0; p.x < width; ++p.x) {
                if constexpr (std::invocable<Func, value_type &, const Pos &>) {
                    func((*this)[p], p);
                } else {
                    func((*this)[p]);
                }
            }
        }
    }

    /**
     * Calls formatter(value[, pos]) for each position in the grid in row-major
     * order, and outputs a newline at the end of each row.
     */
    template <typename Formatter>
        requires std::invocable<Formatter, const value_type &, const Pos &> ||
                 std::invocable<Formatter, const value_type &>
    constexpr std::ostream &custom_print(std::ostream &os,
                                         Formatter &&formatter) const {
        Pos pos;
        for (pos.y = 0; pos.y < height; ++pos.y) {
            for (pos.x = 0; pos.x < width; ++pos.x) {
                if constexpr (std::invocable<Formatter, const value_type &,
                                             const Pos &>) {
                    formatter((*this)[pos], pos);
                } else {
                    formatter((*this)[pos]);
                }
            }
            os << '\n';
        }
        return os;
    }

    /**
     * Calls func(value[, pos]) for each position within the given Chebyshev
     * (8-way) distance from center.
     */
    template <KernelFunc<value_type> Func>
    constexpr void chebyshev_kernel(const Pos &center, int distance,
                                    Func &&func) const {
        Pos pos;
        for (pos.y = center.y - distance; pos.y <= center.y + distance;
             ++pos.y) {
            for (pos.x = center.x - distance; pos.x <= center.x + distance;
                 ++pos.x) {
                if (in_bounds(pos)) {
                    if constexpr (std::invocable<Func, const value_type &,
                                                 const Pos &, int>) {
                        func((*this)[pos], pos,
                             (pos - center).chebyshev_distance());
                    } else if constexpr (std::invocable<Func,
                                                        const value_type &,
                                                        const Pos &>) {
                        func((*this)[pos], pos);
                    } else {
                        func((*this)[pos]);
                    }
                }
            }
        }
    }

    template <KernelFunc<value_type> Func>
    constexpr void chebyshev_kernel(const Pos &center, Func &&func) const {
        chebyshev_kernel(center, 1, std::forward<Func>(func));
    }

    /**
     * Calls func(value[, pos[, distance]]) for each position within the given
     * Manhattan (4-way) distance from center.
     */
    template <KernelFunc<value_type> Func>
    constexpr void manhattan_kernel(const Pos &center, int distance,
                                    Func &&func) const {
        Delta offset{0, 0};
        for (offset.dy = -distance; offset.dy <= distance; ++offset.dy) {
            int x_max = distance - std::abs(offset.dy);
            for (offset.dx = -x_max; offset.dx <= x_max; ++offset.dx) {
                Pos pos = center + offset;
                if (in_bounds(pos)) {
                    if constexpr (std::invocable<Func, const value_type &,
                                                 const Pos &, int>) {
                        func((*this)[pos], pos, offset.manhattan_distance());
                    } else if constexpr (std::invocable<Func,
                                                        const value_type &,
                                                        const Pos &>) {
                        func((*this)[pos], pos);
                    } else {
                        func((*this)[pos]);
                    }
                }
            }
        }
    }

    template <KernelFunc<value_type> Func>
    constexpr void manhattan_kernel(const Pos &center, Func &&func) const {
        manhattan_kernel(center, 1, std::forward<Func>(func));
    }
};

template <class T>
Grid(const std::vector<std::vector<T>> &) -> Grid<T>;
template <class T>
Grid(std::vector<std::vector<T>> &&) -> Grid<T>;

template <class T>
std::ostream &print_repr(std::ostream &os, const aoc::ds::Grid<T> &grid,
                         const pretty_print::repr_state state) {
    // Get the current field width (from std::setw()) which we will use for the
    // individual values. Also set it to 0, as it shouldn't apply to the
    // initial '['.
    auto field_width = os.width(0);
    for (int i = 0; const auto &row : grid) {
        os << (i == 0 ? '[' : ' ') << '[';
        for (int j = 0; const auto &value : row) {
            if (j != 0) {
                os << " ";
            }
            os << std::setw(field_width) << pretty_print::repr(value, state);
            ++j;
        }
        ++i;
        os << ']' << (i == grid.height ? "]" : ",\n");
    }
    return os;
}

// instantiate templates in a non-templated helper function, so static
// analyzers will check these functions
namespace test {
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
    // reduce boilerplate
    constexpr auto same_type = [](const auto &a, const auto &b) {
        return std::same_as<decltype(a), decltype(b)>;
    };
    // construct with the same value everywhere
    {
        Grid<int> int_grid(10, 5);
        Grid int_grid_2(10, 5, 10);
        static_assert(same_type(int_grid, int_grid_2));

        Grid<std::pair<int, std::string>> pair_grid_1(3, 2);
        Grid pair_grid_2(3, 2, std::make_pair(1, std::string{"a"}));
        static_assert(same_type(pair_grid_1, pair_grid_2));
    }

    // construct from a flat range
    {
        std::array<float, 10> flat_arr;
        Grid<float> grid_1(2, 5, flat_arr);
        static_assert(
            std::same_as<typename decltype(grid_1)::value_type, float>);
        std::vector<float> flat_vec(10, 3.14f);
        Grid<float> grid_2(2, 5, flat_vec);
        static_assert(
            std::same_as<typename decltype(grid_2)::value_type, float>);
        static_assert(same_type(grid_1, grid_2));
    }

    // move-construct from a flat data container
    {
        std::vector<std::string> data(15, "foobar");
        Grid<std::string> grid(3, 5, std::move(data));
        static_assert(
            std::same_as<typename decltype(grid)::value_type, std::string>);
    }

    // construct from nested ranges
    {
        std::vector<std::vector<bool>> bool_vec{{true, false}, {true, false}};
        Grid<bool> bool_grid_1(bool_vec);
        Grid bool_grid_2(bool_vec);
        Grid bool_grid_3(std::move(bool_vec));
        static_assert(same_type(bool_grid_1, bool_grid_2));
        static_assert(same_type(bool_grid_1, bool_grid_3));
    }

    // construct with the same dimensions as another grid
    {
        Grid<int> grid_1(3, 4);
        Grid<float> grid_2(grid_1, 0.0);
        assert(grid_1.width == grid_2.width && grid_1.height == grid_2.height);
        Grid<int> grid_3(grid_2, 1);
        assert(grid_1.width == grid_3.width && grid_1.height == grid_3.height);
    }

    // check that iterators satisfy the expected constraints
    {
        constexpr auto check_iterator = []<typename Iterator>() {
            static_assert(std::bidirectional_iterator<Iterator>);
            static_assert(!std::random_access_iterator<Iterator>);
            static_assert(!std::contiguous_iterator<Iterator>);
        };
        check_iterator.template operator()<Grid<int>::iterator>();
        check_iterator.template operator()<Grid<int>::const_iterator>();
        check_iterator.template operator()<Grid<bool>::iterator>();
        check_iterator.template operator()<Grid<bool>::const_iterator>();
    }

    // basic iterator test
    {
        Grid<int> grid(5, 7, 1);
        int i = 0;
        int y = 0;
        for (auto &row : grid) {
            int x = 0;
            for (auto &val : row) {
                val = i;
                assert(grid.at(x, y) == i);
                ++x;
                ++i;
            }
            ++y;
        }
    }

    {
        Grid<int> grid(5, 10);
        grid.for_each([](int) {});
        grid.for_each([](int, const Pos &) {});
        grid.for_each([](int, Pos) {});
    }

    // TODO: add checks for kernel functions

    // an iterator should be assignable to a const_iterator
    static_assert(
        std::convertible_to<Grid<int>::iterator, Grid<int>::const_iterator>);
    static_assert(
        std::convertible_to<Grid<bool>::iterator, Grid<bool>::const_iterator>);

#if __cpp_lib_constexpr_vector
    static_assert(_grid_lint_helper_constexpr());
#endif
}
} // namespace test

} // namespace aoc::ds

template <class T>
struct pretty_print::has_custom_print_repr<aoc::ds::Grid<T>>
    : pretty_print::has_any_print_repr<T> {};

#endif /* end of include guard: GRID_HPP_EF9V6ZF8 */
