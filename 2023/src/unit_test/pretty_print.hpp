#ifndef PRETTY_PRINT_HPP
#define PRETTY_PRINT_HPP

#include "util/concepts.hpp"
#include <array>    // for array
#include <compare>  // for strong_ordering
#include <cstddef>  // for size_t
#include <iomanip>  // for quoted
#include <iostream> // for ostream, defaultfloat, hexfloat
#include <iterator> // for begin, end
#include <list>     // for list
#include <map>      // for map
#include <optional> // for optional
#include <set>      // for set
#include <string>   // for string
#include <tuple>    // for tuple, get
#include <utility>  // for pair
#include <vector>   // for vector

// Additional ostream formatters needed for printing arguments/return values
// for failing test cases

namespace pretty_print {
template <class T>
std::ostream &write_repr(std::ostream &, const T &, const bool result = false);
} // namespace pretty_print

// iterable containers (vector, list, set, etc.)
template <template <class, class...> class C, class T>
    requires(!std::is_same_v<C<T>, std::string> &&
             util::concepts::any_iterable_collection<C<T>, T>)
std::ostream &operator<<(std::ostream &os, const C<T> &container) {
    os << "{";
    for (auto it = container.begin(); it != container.end(); ++it) {
        if (it != container.begin()) {
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

// mappings (map, unordered_map, multimap, etc.)
template <template <class, class, class...> class M, class K, class V>
    requires util::concepts::any_iterable_mapping<M<K, V>, K, V>
std::ostream &operator<<(std::ostream &os, const M<K, V> &mapping) {
    os << "{";
    for (auto it = mapping.begin(); it != mapping.end(); ++it) {
        if (it != mapping.begin()) {
            os << ", ";
        }
        pretty_print::write_repr(os, it->first);
        os << ": ";
        pretty_print::write_repr(os, it->second);
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

// instantiate templates in an anonymous namespace, so static analyzers will
// check these functions
namespace {
template <class T>
void _lint_helper_template(std::ostream &os, const T &t = T()) {
    pretty_print::write_repr(os, t, true);
    pretty_print::write_repr(os, t, false);
}
[[maybe_unused]] void _lint_helper(std::ostream &os) {
    _lint_helper_template<int>(os);
    _lint_helper_template<bool>(os);
    _lint_helper_template<double>(os);
    _lint_helper_template<std::string>(os);
    _lint_helper_template<std::vector<int>>(os);
    _lint_helper_template<std::list<std::string>>(os);
    _lint_helper_template<std::set<long>>(os);
    _lint_helper_template<std::array<short, 4>>(os);
    _lint_helper_template<std::map<int, bool>>(os);
    _lint_helper_template<std::pair<char, float>>(os);
    _lint_helper_template<std::tuple<std::string, long, double>>(os);
    _lint_helper_template<std::optional<int>>(os);
    _lint_helper_template(os, std::strong_ordering::equal);
}
} // namespace

#endif /* end of include guard: PRETTY_PRINT_HPP */
