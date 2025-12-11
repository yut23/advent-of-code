/******************************************************************************
 * File:        lib.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2022-12-01
 * Description: Common functions used in Advent of Code.
 *****************************************************************************/

#ifndef LIB_HPP_0IZKV7KG
#define LIB_HPP_0IZKV7KG

#include "util/concepts.hpp" // for same_as_any
#include "util/hash.hpp"     // for make_hash
#include "util/util.hpp"     // for reveal_type  // IWYU pragma: keep
#include <algorithm>         // for max, min  // IWYU pragma: keep
#include <cassert>           // for assert
#include <compare>           // for strong_ordering
#include <concepts>          // for integral
#include <cstdlib>           // for abs, size_t, exit
#include <fstream>           // for ifstream  // IWYU pragma: keep
#include <functional>        // for hash
#include <initializer_list>  // for initializer_list
#include <iostream>          // for cout
#include <iterator>          // for istreambuf_iterator
#include <string>            // for string, operator==
#include <type_traits> // for underlying_type_t, is_same_v, is_signed_v, conditional_t, is_const_v
#include <utility> // for move
#include <vector>  // for vector

namespace aoc {

#ifdef DEBUG_MODE
[[maybe_unused]] constexpr bool DEBUG = true;
#else
[[maybe_unused]] constexpr bool DEBUG = false;
#endif

#ifdef FAST_MODE
[[maybe_unused]] constexpr bool FAST = true;
#else
[[maybe_unused]] constexpr bool FAST = false;
#endif

enum class AbsDirection : unsigned char {
    north = 0,
    east = 1,
    south = 2,
    west = 3,
};
[[maybe_unused]] constexpr std::initializer_list<AbsDirection> DIRECTIONS = {
    AbsDirection::north,
    AbsDirection::east,
    AbsDirection::south,
    AbsDirection::west,
};

std::istream &operator>>(std::istream &is, AbsDirection &dir) {
    char ch;
    if (is >> ch) {
        switch (ch) {
        case 'N':
        case 'U':
            dir = AbsDirection::north;
            break;
        case 'E':
        case 'R':
            dir = AbsDirection::east;
            break;
        case 'S':
        case 'D':
            dir = AbsDirection::south;
            break;
        case 'W':
        case 'L':
            dir = AbsDirection::west;
            break;
        default:
            is.setstate(std::ios_base::failbit);
            break;
        }
    }
    return is;
}

std::ostream &operator<<(std::ostream &os, const AbsDirection &dir) {
    switch (dir) {
    case AbsDirection::north:
        os << 'N';
        break;
    case AbsDirection::east:
        os << 'E';
        break;
    case AbsDirection::south:
        os << 'S';
        break;
    case AbsDirection::west:
        os << 'W';
        break;
    }
    return os;
}

enum class RelDirection : unsigned char {
    forward = 0,
    right = 1,
    backward = 2,
    left = 3,
};

std::ostream &operator<<(std::ostream &os, const RelDirection &dir) {
    switch (dir) {
    case RelDirection::forward:
        os << "forward";
        break;
    case RelDirection::right:
        os << "right";
        break;
    case RelDirection::backward:
        os << "backward";
        break;
    case RelDirection::left:
        os << "left";
        break;
    }
    return os;
}

namespace directions {
AbsDirection turn(AbsDirection dir, RelDirection turn_dir) {
    return static_cast<AbsDirection>(
        (static_cast<std::underlying_type_t<AbsDirection>>(dir) +
         static_cast<std::underlying_type_t<RelDirection>>(turn_dir)) %
        4);
}

AbsDirection opposite(AbsDirection dir) {
    return turn(dir, RelDirection::backward);
}

RelDirection relative_to(AbsDirection old_dir, AbsDirection new_dir) {
    return static_cast<RelDirection>(
        (static_cast<std::underlying_type_t<AbsDirection>>(new_dir) -
         static_cast<std::underlying_type_t<AbsDirection>>(old_dir) + 4) %
        4);
}
} // namespace directions

template <std::integral T = int>
struct GenericDelta {
    using int_type = T;
    int_type dx;
    int_type dy;

    constexpr GenericDelta(int_type dx, int_type dy) : dx(dx), dy(dy) {}
    explicit constexpr GenericDelta(AbsDirection dir,
                                    bool invert_vertical = false)
        : dx(0), dy(0) {
        switch (dir) {
        case AbsDirection::north:
            dy = 1;
            break;
        case AbsDirection::south:
            dy = -1;
            break;
        case AbsDirection::east:
            dx = 1;
            break;
        case AbsDirection::west:
            dx = -1;
            break;
        }
        if (invert_vertical) {
            dy *= -1;
        }
    }

    constexpr int_type chebyshev_distance() const {
        return std::max(std::abs(dx), std::abs(dy));
    }

    constexpr int_type manhattan_distance() const {
        return std::abs(dx) + std::abs(dy);
    }

    constexpr int_type euclidean_distance_sq() const {
        return dx * dx + dy * dy;
    }

    // two Deltas can be added together
    GenericDelta &operator+=(const GenericDelta &rhs) {
        dx += rhs.dx;
        dy += rhs.dy;
        return *this;
    }

    // two Deltas can be subtracted
    GenericDelta &operator-=(const GenericDelta &rhs) {
        dx -= rhs.dx;
        dy -= rhs.dy;
        return *this;
    }

    // can be scaled by an integer
    GenericDelta &operator*=(int_type rhs) {
        dx *= rhs;
        dy *= rhs;
        return *this;
    }
    GenericDelta &operator/=(int_type rhs) {
        dx /= rhs;
        dy /= rhs;
        return *this;
    }
};
// this takes lhs by copy, so it doesn't modify the original lhs
template <std::integral int_type>
inline GenericDelta<int_type> operator+(GenericDelta<int_type> lhs,
                                        const GenericDelta<int_type> &rhs) {
    lhs += rhs;
    return lhs;
}
template <std::integral int_type>
inline GenericDelta<int_type> operator-(GenericDelta<int_type> lhs,
                                        const GenericDelta<int_type> &rhs) {
    lhs -= rhs;
    return lhs;
}
template <std::integral int_type>
inline GenericDelta<int_type> operator*(GenericDelta<int_type> lhs,
                                        int_type rhs) {
    lhs *= rhs;
    return lhs;
}
template <std::integral int_type>
inline GenericDelta<int_type> operator*(int_type lhs,
                                        GenericDelta<int_type> rhs) {
    rhs *= lhs;
    return rhs;
}
template <std::integral int_type>
inline GenericDelta<int_type> operator/(GenericDelta<int_type> lhs,
                                        int_type rhs) {
    lhs /= rhs;
    return lhs;
}
// negation operator
template <std::integral int_type>
inline GenericDelta<int_type> operator-(GenericDelta<int_type> delta) {
    delta.dx *= -1;
    delta.dy *= -1;
    return delta;
}

template <std::integral int_type>
std::ostream &operator<<(std::ostream &os,
                         const GenericDelta<int_type> &delta) {
    os << "Delta(" << delta.dx << ", " << delta.dy << ")";
    return os;
}

template <std::integral T = int>
struct GenericPos {
    using int_type = T;
    int_type x;
    int_type y;

    constexpr GenericPos() : x(0), y(0) {}
    constexpr GenericPos(int_type x, int_type y) : x(x), y(y) {}

    // we can add a Delta to a Pos, but not another Pos
    GenericPos &operator+=(const GenericDelta<int_type> &rhs) {
        x += rhs.dx;
        y += rhs.dy;
        return *this;
    }
    GenericPos &operator-=(const GenericDelta<int_type> &rhs) {
        x -= rhs.dx;
        y -= rhs.dy;
        return *this;
    }

    // can scale by an integer
    GenericPos &operator*=(int_type rhs) {
        x *= rhs;
        y *= rhs;
        return *this;
    }
    GenericPos &operator/=(int_type rhs) {
        x /= rhs;
        y /= rhs;
        return *this;
    }

    constexpr std::strong_ordering
    operator<=>(const GenericPos<int_type> &) const = default;
};
// this takes lhs by copy, so it doesn't modify the original lhs
template <std::integral int_type>
inline GenericPos<int_type> operator+(GenericPos<int_type> lhs,
                                      const GenericDelta<int_type> &rhs) {
    lhs += rhs;
    return lhs;
}
template <std::integral int_type>
inline GenericPos<int_type> operator-(GenericPos<int_type> lhs,
                                      const GenericDelta<int_type> &rhs) {
    lhs -= rhs;
    return lhs;
}
template <std::integral int_type>
inline GenericPos<int_type> operator*(GenericPos<int_type> lhs, int rhs) {
    lhs *= rhs;
    return lhs;
}
template <std::integral int_type>
inline GenericPos<int_type> operator/(GenericPos<int_type> lhs, int rhs) {
    lhs /= rhs;
    return lhs;
}
// can subtract two Pos, yielding a Delta
template <std::integral int_type>
inline GenericDelta<int_type> operator-(const GenericPos<int_type> &lhs,
                                        const GenericPos<int_type> &rhs) {
    return {lhs.x - rhs.x, lhs.y - rhs.y};
}

template <std::integral int_type>
std::ostream &operator<<(std::ostream &os, const GenericPos<int_type> &pos) {
    os << "Pos(" << pos.x << ", " << pos.y << ")";
    return os;
}

using Delta = GenericDelta<int>;
using Pos = GenericPos<int>;

using LongDelta = GenericDelta<long>;
using LongPos = GenericPos<long>;

template <std::integral T = int>
struct GenericDelta3 {
    using int_type = T;
    int_type dx;
    int_type dy;
    int_type dz;

    constexpr GenericDelta3(int_type dx, int_type dy, int_type dz)
        : dx(dx), dy(dy), dz(dz) {}

    constexpr int_type chebyshev_distance() const {
        return std::max(std::abs(dx), std::abs(dy), std::abs(dz));
    }

    constexpr int_type manhattan_distance() const {
        return std::abs(dx) + std::abs(dy) + std::abs(dz);
    }

    constexpr int_type euclidean_distance_sq() const {
        return dx * dx + dy * dy + dz * dz;
    }

    // two Deltas can be added together
    GenericDelta3 &operator+=(const GenericDelta3 &rhs) {
        dx += rhs.dx;
        dy += rhs.dy;
        dz += rhs.dz;
        return *this;
    }

    // two Deltas can be subtracted
    GenericDelta3 &operator-=(const GenericDelta3 &rhs) {
        dx -= rhs.dx;
        dy -= rhs.dy;
        dz -= rhs.dz;
        return *this;
    }

    // can be scaled by an integer
    GenericDelta3 &operator*=(int_type rhs) {
        dx *= rhs;
        dy *= rhs;
        dz *= rhs;
        return *this;
    }
    GenericDelta3 &operator/=(int_type rhs) {
        dx /= rhs;
        dy /= rhs;
        dz /= rhs;
        return *this;
    }
};
// this takes lhs by copy, so it doesn't modify the original lhs
template <std::integral int_type>
inline GenericDelta3<int_type> operator+(GenericDelta3<int_type> lhs,
                                         const GenericDelta3<int_type> &rhs) {
    lhs += rhs;
    return lhs;
}
template <std::integral int_type>
inline GenericDelta3<int_type> operator-(GenericDelta3<int_type> lhs,
                                         const GenericDelta3<int_type> &rhs) {
    lhs -= rhs;
    return lhs;
}
template <std::integral int_type>
inline GenericDelta3<int_type> operator*(GenericDelta3<int_type> lhs,
                                         int_type rhs) {
    lhs *= rhs;
    return lhs;
}
template <std::integral int_type>
inline GenericDelta3<int_type> operator*(int_type lhs,
                                         GenericDelta3<int_type> rhs) {
    rhs *= lhs;
    return rhs;
}
template <std::integral int_type>
inline GenericDelta3<int_type> operator/(GenericDelta3<int_type> lhs,
                                         int_type rhs) {
    lhs /= rhs;
    return lhs;
}
// negation operator
template <std::integral int_type>
inline GenericDelta3<int_type> operator-(GenericDelta3<int_type> delta) {
    delta.dx *= -1;
    delta.dy *= -1;
    delta.dz *= -1;
    return delta;
}

template <std::integral int_type>
std::ostream &operator<<(std::ostream &os,
                         const GenericDelta3<int_type> &delta) {
    os << "Delta3(" << delta.dx << ", " << delta.dy << ", " << delta.dz << ")";
    return os;
}

template <std::integral T = int>
struct GenericPos3 {
    using int_type = T;
    int_type x;
    int_type y;
    int_type z;

    constexpr GenericPos3() : x(0), y(0), z(0) {}
    constexpr GenericPos3(int_type x, int_type y, int_type z)
        : x(x), y(y), z(z) {}

    // we can add a Delta3 to a Pos3, but not another Pos3
    GenericPos3 &operator+=(const GenericDelta3<int_type> &rhs) {
        x += rhs.dx;
        y += rhs.dy;
        z += rhs.dz;
        return *this;
    }
    GenericPos3 &operator-=(const GenericDelta3<int_type> &rhs) {
        x -= rhs.dx;
        y -= rhs.dy;
        z -= rhs.dz;
        return *this;
    }

    // can scale by an integer
    GenericPos3 &operator*=(int_type rhs) {
        x *= rhs;
        y *= rhs;
        z *= rhs;
        return *this;
    }
    GenericPos3 &operator/=(int_type rhs) {
        x /= rhs;
        y /= rhs;
        z /= rhs;
        return *this;
    }

    constexpr std::strong_ordering
    operator<=>(const GenericPos3<int_type> &) const = default;
};
// this takes lhs by copy, so it doesn't modify the original lhs
template <std::integral int_type>
inline GenericPos3<int_type> operator+(GenericPos3<int_type> lhs,
                                       const GenericDelta3<int_type> &rhs) {
    lhs += rhs;
    return lhs;
}
template <std::integral int_type>
inline GenericPos3<int_type> operator-(GenericPos3<int_type> lhs,
                                       const GenericDelta3<int_type> &rhs) {
    lhs -= rhs;
    return lhs;
}
template <std::integral int_type>
inline GenericPos3<int_type> operator*(GenericPos3<int_type> lhs, int rhs) {
    lhs *= rhs;
    return lhs;
}
template <std::integral int_type>
inline GenericPos3<int_type> operator/(GenericPos3<int_type> lhs, int rhs) {
    lhs /= rhs;
    return lhs;
}
// can subtract two Pos3, yielding a Delta3
template <std::integral int_type>
inline GenericDelta3<int_type> operator-(const GenericPos3<int_type> &lhs,
                                         const GenericPos3<int_type> &rhs) {
    return {lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
}

template <std::integral int_type>
std::ostream &operator<<(std::ostream &os, const GenericPos3<int_type> &pos) {
    os << "Pos3(" << pos.x << ", " << pos.y << ", " << pos.z << ")";
    return os;
}

using Delta3 = GenericDelta3<int>;
using Pos3 = GenericPos3<int>;

using LongDelta3 = GenericDelta3<long>;
using LongPos3 = GenericPos3<long>;

template <class T>
T intersect_ranges(const T &range_1, const T &range_2) {
    const auto &[r1_lo, r1_hi] = range_1;
    const auto &[r2_lo, r2_hi] = range_2;
    auto lo = std::max(r1_lo, r2_lo);
    auto hi = std::min(r1_hi, r2_hi);
    return {lo, std::max(hi, lo - 1)};
}

template <class T>
bool ranges_overlap(const T &range_1, const T &range_2) {
    const auto &[r1_lo, r1_hi] = range_1;
    const auto &[r2_lo, r2_hi] = range_2;
    auto lo = std::max(r1_lo, r2_lo);
    auto hi = std::min(r1_hi, r2_hi);
    return lo <= hi;
}

// use an anonymous namespace to hide these implementation details
namespace {
template <typename T>
struct SkipInputHelper {
    int count;
    friend std::istream &operator>>(std::istream &is, SkipInputHelper s) {
        T temp;
        for (int i = 0; i < s.count && (is >> temp); ++i)
            ;
        return is;
    }
};

template <class T>
struct ExpectInputHelper {
    T expected;
    friend std::istream &operator>>(std::istream &is, ExpectInputHelper &&e) {
        T temp{};
        if constexpr (std::is_same_v<T, std::string>) {
            char ch;
            for (std::size_t i = 0; i < e.expected.size() && is >> ch; ++i) {
                temp.push_back(ch);
            }
        } else {
            is >> temp;
        }
        if (!is || temp != e.expected) {
            is.setstate(std::ios_base::failbit);
        }
        return is;
    }

    explicit ExpectInputHelper(const T &expected) : expected(expected) {}
    explicit ExpectInputHelper(T &&expected) : expected(std::move(expected)) {}
};
} // namespace

template <typename T = std::string>
SkipInputHelper<T> skip(int count = 1) {
    return SkipInputHelper<T>{count};
}

template <class T>
ExpectInputHelper<std::remove_cvref_t<T>> expect_input(T &&expected) {
    return ExpectInputHelper<std::remove_cvref_t<T>>(std::forward<T>(expected));
}
ExpectInputHelper<std::string> expect_input(const char *expected) {
    return ExpectInputHelper<std::string>{std::string{expected}};
}

// I/O manipulator that extracts an 8-bit number into a char instead of
// a single ASCII character, and inserts a char as an 8-bit number.
template <typename T>
struct as_number {
    T &dest;
    friend std::istream &operator>>(std::istream &is, as_number h) {
        static_assert(!std::is_const_v<T>,
                      "Cannot extract into a const reference");
        constexpr bool is_char =
            util::concepts::same_as_any<T, char, signed char, unsigned char>;
        if constexpr (is_char) {
            using int_type =
                std::conditional_t<std::is_signed_v<T>, short, unsigned short>;
            int_type temp;
            is >> temp;
            // cppcheck-suppress unreadVariable
            h.dest = static_cast<T>(temp);
        } else {
            is >> h.dest;
        }
        return is;
    }

    friend std::ostream &operator<<(std::ostream &os, const as_number &h) {
        constexpr bool is_char =
            util::concepts::same_as_any<std::remove_const_t<T>, char,
                                        signed char, unsigned char>;
        if constexpr (is_char) {
            // all signed or unsigned 8-bit values will fit in a short
            os << static_cast<short>(h.dest);
        } else {
            os << h.dest;
        }
        return os;
    }

    explicit as_number(T &dest) : dest(dest) {}
};

template <class T>
as_number(const T &) -> as_number<const T>;

/**
 * Advent of Code problem part number.
 */
enum class Part : unsigned char {
    PART_1 = 1,
    PART_2 = 2,
};
// make PART_1 and PART_2 available in the top-level namespace, for less typing
// (I'm just using a scoped enum for type safety)
using enum Part;

enum class InputType {
    OTHER,
    EXAMPLE,
    MAIN,
};
struct Arguments {
    std::ifstream infile;
    std::string filename;
    InputType input_type;
};

/**
 * @brief Parse command line arguments.
 */
Arguments parse_args(int argc, char **argv) {
    if (argc != 2) {
        assert(argc >= 1);
        std::cout << "Usage: " << argv[0] << " <input file path>" << std::endl;
        std::exit(1);
    }
    Arguments args{
        .infile = std::ifstream{argv[1]},
        .filename = std::string{argv[1]},
        .input_type = InputType::OTHER,
    };
    if (args.filename.ends_with("input.txt")) {
        args.input_type = InputType::MAIN;
    } else if (args.filename.find("example") != std::string::npos) {
        args.input_type = InputType::EXAMPLE;
    }
    return args;
}

/**
 * @brief Reads an entire stream into a string.
 */
std::string read_whole_stream(std::istream &is) {
    return std::string{std::istreambuf_iterator<char>(is), {}};
}

/**
 * @brief Reads lines from a stream.
 */
std::vector<std::string> read_lines(std::istream &is) {
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(is, line)) {
        lines.push_back(std::move(line));
    }
    return lines;
}

/**
 * @brief Reads arbitrary objects from a stream into a vector.
 */
template <typename T>
std::vector<T> read_vector(std::istream &is) {
    std::vector<T> vec;
    T t{};
    while (is >> t) {
        vec.push_back(std::move(t));
    }
    return vec;
}

/**
 * @brief Reads comma-separated values from a stream into a vector.
 *
 * Does not handle quoting.
 */
template <typename T>
std::vector<T> read_csv(std::istream &is) {
    std::vector<T> vec;
    T t{};
    while (true) {
        if (is >> t) {
            vec.push_back(std::move(t));
        } else {
            break;
        }
        // peek at the next character
        char c = is.peek();
        if (c == ',') {
            // consume the comma
            is.get();
            continue;
        }
        // next character isn't a comma, so stop
        break;
    }
    return vec;
}

template <typename T>
struct csv_formatter {
    using value_type = T;
    using clean_value_type = std::remove_cvref_t<T>;
    using container_type = std::vector<clean_value_type>;
    std::conditional_t<std::is_const_v<T>, const container_type, container_type>
        &dest;

    friend std::istream &operator>>(std::istream &is, csv_formatter fmt) {
        static_assert(!std::is_const_v<T>,
                      "Cannot extract into a const reference");
        auto vec = aoc::read_csv<clean_value_type>(is);
        if (is) {
            // cppcheck-suppress unreadVariable
            fmt.dest = std::move(vec);
        }
        return is;
    }

    friend std::ostream &operator<<(std::ostream &os,
                                    const csv_formatter &fmt) {
        bool first = true;
        for (const auto &value : fmt.dest) {
            if (!first) {
                os << ',';
            }
            os << value;
            first = false;
        }
        return os;
    }

    explicit csv_formatter(container_type &dest) : dest(dest) {}
    explicit csv_formatter(const container_type &dest) : dest(dest) {}
};

template <class T>
csv_formatter(std::vector<T> &) -> csv_formatter<T>;
template <class T>
csv_formatter(const std::vector<T> &) -> csv_formatter<const T>;

} // namespace aoc

template <std::integral int_type>
struct std::hash<aoc::GenericPos<int_type>> {
    std::size_t
    operator()(const aoc::GenericPos<int_type> &pos) const noexcept {
        // random number (hexdump -n8 -e '"0x" 8/1 "%02x" "ull\n"'</dev/urandom)
        std::size_t seed = 0xbedb5bb0b473b6b7ull;
        util::make_hash(seed, pos.x, pos.y);
        return seed;
    }
};

template <std::integral int_type>
struct std::hash<aoc::GenericPos3<int_type>> {
    std::size_t
    operator()(const aoc::GenericPos3<int_type> &pos) const noexcept {
        // random number (hexdump -n8 -e '"0x" 8/1 "%02x" "ull\n"'</dev/urandom)
        std::size_t seed = 0x3d5fe89106c6a5cfull;
        util::make_hash(seed, pos.x, pos.y, pos.z);
        return seed;
    }
};

// instantiate templates in an anonymous namespace, so static analyzers will
// check these functions
namespace {
[[maybe_unused]] void _lint_helper(std::istream &is, std::ostream &os) {
    is >> aoc::skip<char>(2);
    is >> aoc::skip<int>(1);
    is >> aoc::skip<std::string>(3);

    is >> aoc::expect_input('a');
    is >> aoc::expect_input("foo");
    is >> aoc::expect_input<std::string>("foo");
    is >> aoc::expect_input(std::string{"bar"});

    char ch{};
    unsigned char uch{};
    is >> aoc::as_number(ch);
    is >> aoc::as_number(uch);

    os << aoc::as_number(ch);
    os << aoc::as_number(uch);
    os << aoc::as_number('a');
}
} // namespace

#endif /* end of include guard: LIB_HPP_0IZKV7KG */
