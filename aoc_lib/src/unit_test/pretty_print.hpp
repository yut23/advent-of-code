#ifndef PRETTY_PRINT_HPP
#define PRETTY_PRINT_HPP

#include "util/concepts.hpp"
#include <array>       // for array
#include <cctype>      // for isprint
#include <compare>     // for strong_ordering
#include <concepts>    // for same_as, convertible_to, floating_point
#include <cstddef>     // for size_t
#include <iostream>    // for ostream, ios, defaultfloat, hexfloat
#include <iterator>    // for begin, end, iter_value_t
#include <list>        // for list
#include <map>         // for map
#include <optional>    // for optional
#include <set>         // for set
#include <string>      // for string
#include <string_view> // for string_view
#include <tuple>       // for tuple, get
#include <type_traits> // for true_type, false_type, remove_cvref_t, integral_constant
#include <utility> // for declval, index_sequence, index_sequence_for, pair
#include <vector>  // for vector

// Additional ostream formatters needed for printing arguments/return values
// for failing test cases

namespace pretty_print {
template <class T>
class repr;

template <class T>
struct has_custom_print_repr : std::false_type {};

template <class T>
struct has_any_print_repr
    : std::integral_constant<
          bool, util::concepts::stream_insertable<T> ||
                    has_custom_print_repr<std::remove_cvref_t<T>>::value> {};

template <class T>
concept has_print_repr = has_any_print_repr<T>::value;
} // namespace pretty_print

/*
 * Templated types
 */

// iterable containers (vector, list, set, etc.)
template <template <class, class...> class C, class T>
    requires util::concepts::any_iterable_collection<C<T>, T> &&
             pretty_print::has_print_repr<
                 std::iter_value_t<decltype(std::declval<C<T>>().begin())>> &&
             (!std::convertible_to<C<T>, std::string_view>)
struct pretty_print::has_custom_print_repr<C<T>> : std::true_type {};

template <template <class, class...> class C, class T>
    requires util::concepts::any_iterable_collection<C<T>, T> &&
             pretty_print::has_print_repr<
                 std::iter_value_t<decltype(std::declval<C<T>>().begin())>> &&
             (!std::convertible_to<C<T>, std::string_view>)
std::ostream &print_repr(std::ostream &os, const C<T> &container,
                         const bool result) {
    os << "{";
    for (auto it = container.begin(); it != container.end(); ++it) {
        if (it != container.begin()) {
            os << ", ";
        }
        os << pretty_print::repr(*it, result);
    }
    os << "}";
    return os;
}

// arrays
template <pretty_print::has_print_repr T, std::size_t N>
struct pretty_print::has_custom_print_repr<std::array<T, N>> : std::true_type {
};

template <pretty_print::has_print_repr T, std::size_t N>
std::ostream &print_repr(std::ostream &os, const std::array<T, N> &arr,
                         const bool result) {
    os << "{";
    for (auto it = std::begin(arr); it != std::end(arr); ++it) {
        if (it != std::begin(arr)) {
            os << ", ";
        }
        os << pretty_print::repr(*it, result);
    }
    os << "}";
    return os;
}

// mappings (map, unordered_map, multimap, etc.)
template <template <class, class, class...> class M,
          pretty_print::has_print_repr K, pretty_print::has_print_repr V>
    requires util::concepts::any_iterable_mapping<M<K, V>, K, V>
struct pretty_print::has_custom_print_repr<M<K, V>> : std::true_type {};

template <template <class, class, class...> class M,
          pretty_print::has_print_repr K, pretty_print::has_print_repr V>
    requires util::concepts::any_iterable_mapping<M<K, V>, K, V>
std::ostream &print_repr(std::ostream &os, const M<K, V> &mapping,
                         const bool result) {
    using pretty_print::repr;
    os << "{";
    for (auto it = mapping.begin(); it != mapping.end(); ++it) {
        if (it != mapping.begin()) {
            os << ", ";
        }
        os << repr(it->first, result) << ": " << repr(it->second, result);
    }
    os << "}";
    return os;
}

// tuple-like containers (tuple, pair)
template <template <class...> class C, pretty_print::has_print_repr... Ts>
    requires std::same_as<C<Ts...>, std::tuple<Ts...>> ||
             std::same_as<C<Ts...>, std::pair<Ts...>>
struct pretty_print::has_custom_print_repr<C<Ts...>> : std::true_type {};

template <template <class...> class C, pretty_print::has_print_repr... Ts>
    requires std::same_as<C<Ts...>, std::tuple<Ts...>> ||
             std::same_as<C<Ts...>, std::pair<Ts...>>
std::ostream &print_repr(std::ostream &os, const C<Ts...> &args,
                         const bool result) {
    os << "[";
    [&]<std::size_t... I>(std::index_sequence<I...>) {
        ((os << (I == 0 ? "" : ", ")
             << pretty_print::repr(std::get<I>(args), result)),
         ...);
    }(std::index_sequence_for<Ts...>{});
    os << "]";
    return os;
}

// optional
template <pretty_print::has_print_repr T>
struct pretty_print::has_custom_print_repr<std::optional<T>> : std::true_type {
};

template <pretty_print::has_print_repr T>
std::ostream &print_repr(std::ostream &os, const std::optional<T> &opt,
                         const bool result) {
    if (opt.has_value()) {
        os << pretty_print::repr(*opt, result);
    } else {
        os << "{}";
    }
    return os;
}

/*
 * Base/leaf types
 */

// strong_ordering
template <>
struct pretty_print::has_custom_print_repr<std::strong_ordering>
    : std::true_type {};

std::ostream &print_repr(std::ostream &os, const std::strong_ordering &value,
                         const bool /*result*/) {
    if (value < 0) {
        os << "less";
    } else if (value > 0) {
        os << "greater";
    } else {
        os << "equal";
    }
    return os;
}

// exactly bool
template <>
struct pretty_print::has_custom_print_repr<bool> : std::true_type {};

template <std::same_as<bool> B>
std::ostream &print_repr(std::ostream &os, B b, const bool /*result*/) {
    os << (b ? "true" : "false");
    return os;
}

// helper function to add backslash escapes for characters
template <util::concepts::same_as_any<char, signed char, unsigned char> C>
void write_escaped_char(std::ostream &os, C ch, char quote_char) {
    if (std::isprint(static_cast<unsigned char>(ch))) {
        if (ch == '\\' || ch == quote_char) {
            os << '\\';
        }
        os << ch;
    } else {
        os << '\\';
        switch (ch) {
        case '\t':
            os << 't';
            break;
        case '\n':
            os << 'n';
            break;
        case '\r':
            os << 'r';
            break;
        default: {
            // hex escape
            os << 'x';
            auto orig_flags = os.flags(std::ios::hex | std::ios::right);
            auto orig_width = os.width(2);
            auto orig_fill = os.fill('0');
            os << (static_cast<unsigned int>(ch) & 0xFF);
            os.flags(orig_flags);
            os.width(orig_width);
            os.fill(orig_fill);
        }
        }
    }
}

// strings (anything that's implicitly convertible to a string_view)
template <std::convertible_to<std::string_view> S>
struct pretty_print::has_custom_print_repr<S> : std::true_type {};

std::ostream &print_repr(std::ostream &os, std::string_view sv,
                         const bool /*result*/) {
    os << '"';
    for (char ch : sv) {
        write_escaped_char(os, ch, '"');
    }
    os << '"';
    return os;
}

// (signed/unsigned) char
template <util::concepts::same_as_any<char, signed char, unsigned char> C>
struct pretty_print::has_custom_print_repr<C> : std::true_type {};

template <util::concepts::same_as_any<char, signed char, unsigned char> C>
std::ostream &print_repr(std::ostream &os, C ch, const bool /*result*/) {
    os << '\'';
    write_escaped_char(os, ch, '\'');
    os << '\'';
    return os;
}

// floating point values
template <std::floating_point FP>
struct pretty_print::has_custom_print_repr<FP> : std::true_type {};

template <std::floating_point FP>
std::ostream &print_repr(std::ostream &os, FP x, const bool result) {
    os << x;
    if (result) {
        os << " (" << std::hexfloat << x << std::defaultfloat << ")";
    }
    return os;
}

// anything else
template <class T>
    requires(
        util::concepts::stream_insertable<T> &&
        !pretty_print::has_custom_print_repr<std::remove_cvref_t<T>>::value)
std::ostream &print_repr(std::ostream &os, const T &t, const bool /*result*/) {
    os << t;
    return os;
}

namespace pretty_print {

template <class T>
class repr {
    static_assert(has_print_repr<T>, "missing stream insertion operator (<<)");

    const T &val;
    bool result;
    friend std::ostream &operator<<(std::ostream &os, const repr &r) {
        print_repr(os, r.val, r.result);
        return os;
    }

  public:
    explicit repr(const T &val, bool result = false)
        : val(val), result(result) {}
};

} // namespace pretty_print

// instantiate templates in an anonymous namespace, so static analyzers will
// check these functions
namespace {
template <class T>
void _lint_helper_template(std::ostream &os, const T &t = T()) {
    print_repr(os, t, true);
    print_repr(os, t, false);
    os << pretty_print::repr(t);
    os << pretty_print::repr(t, true);
    os << pretty_print::repr(t, false);
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
