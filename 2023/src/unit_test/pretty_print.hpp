#ifndef PRETTY_PRINT_HPP
#define PRETTY_PRINT_HPP

#include <array>    // for array // IWYU pragma: keep
#include <compare>  // for strong_ordering
#include <cstddef>  // for size_t
#include <iomanip>  // for quoted
#include <iostream> // for ostream, defaultfloat, hexfloat
#include <iterator> // for begin, end
#include <list>     // for vector // IWYU pragma: keep
#include <optional> // for optional
#include <ranges>   // for range
#include <set>      // for vector // IWYU pragma: keep
#include <string>   // for string
#include <tuple>    // for tuple, get
#include <utility>  // for pair
#include <vector>   // for vector // IWYU pragma: keep

// Additional ostream formatters needed for printing arguments/return values
// for failing test cases

namespace pretty_print {
template <class T>
std::ostream &write_repr(std::ostream &, const T &, const bool result = false);
};

// iterable containers (vector, list, set, etc.)
template <template <class, class...> class C, class T>
    requires(!std::is_same_v<C<T>, std::string> && std::ranges::range<C<T>>)
std::ostream &operator<<(std::ostream &os, const C<T> &container) {
    os << "{";
    for (auto it = std::begin(container); it != std::end(container); ++it) {
        if (it != std::begin(container)) {
            os << ", ";
        }
        pretty_print::write_repr(os, *it);
    }
    os << "}";
    return os;
}

// arrays
template <class T, std::size_t N>
std::ostream &operator<<(std::ostream &os, const std::array<T, N> &arr) {
    os << "{";
    for (auto it = std::begin(arr); it != std::end(arr); ++it) {
        if (it != std::begin(arr)) {
            os << ", ";
        }
        pretty_print::write_repr(os, *it);
    }
    os << "}";
    return os;
}

// tuple-like containers (tuple, pair)
template <template <class...> class C, class... Ts>
    requires std::is_same_v<C<Ts...>, std::tuple<Ts...>> ||
             std::is_same_v<C<Ts...>, std::pair<Ts...>>
std::ostream &operator<<(std::ostream &os, const C<Ts...> &args) {
    os << "[";
    []<std::size_t... I>(std::ostream & os, const C<Ts...> &args,
                         std::index_sequence<I...>) {
        (pretty_print::write_repr(os << (I == 0 ? "" : ", "),
                                  std::get<I>(args)),
         ...);
    }
    (os, args, std::index_sequence_for<Ts...>{});
    os << "]";
    return os;
}

// optional
template <class T>
std::ostream &operator<<(std::ostream &os, const std::optional<T> &opt) {
    if (opt.has_value()) {
        os << *opt;
    } else {
        os << "{}";
    }
    return os;
}

// strong_ordering
std::ostream &operator<<(std::ostream &os, const std::strong_ordering &value) {
    if (value < 0) {
        os << "less";
    } else if (value > 0) {
        os << "greater";
    } else {
        os << "equal";
    }
    return os;
}

// Special formatting for printing leaf types

namespace pretty_print {

template <class T>
std::ostream &write_repr(std::ostream &os, const T &val, const bool result) {
    if constexpr (std::is_same_v<T, std::string>) {
        os << std::quoted(val);
    } else if constexpr (std::is_same_v<T, bool>) {
        os << (val ? "true" : "false");
    } else if (std::is_floating_point_v<T> && result) {
        os << val << " (" << std::hexfloat << val << std::defaultfloat << ")";
    } else {
        os << val;
    }
    return os;
}

} // namespace pretty_print

#endif /* end of include guard: PRETTY_PRINT_HPP */
