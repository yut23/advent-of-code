/******************************************************************************
 * File:        lib.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2022-12-01
 * Description: Common functions used in Advent of Code.
 *****************************************************************************/

#ifndef LIB_HPP_0IZKV7KG
#define LIB_HPP_0IZKV7KG

#include <algorithm>        // for max, min
#include <cassert>          // for assert
#include <compare>          // for strong_ordering
#include <cstdlib>          // for abs, size_t, exit
#include <fstream>          // for ifstream  // IWYU pragma: keep
#include <functional>       // for hash
#include <initializer_list> // for initializer_list
#include <iostream>         // for cout
#include <string>           // for string
#include <type_traits> // for underlying_type_t, is_same_v, is_signed_v, conditional_t, is_const_v
#include <utility> // for move

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

struct Delta {
    int dx;
    int dy;

    constexpr Delta(int dx, int dy) : dx(dx), dy(dy) {}
    explicit constexpr Delta(AbsDirection dir, bool invert_vertical = false)
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

    constexpr int chebyshev_distance() const {
        return std::max(std::abs(dx), std::abs(dy));
    }

    constexpr int manhattan_distance() const {
        return std::abs(dx) + std::abs(dy);
    }

    // two Deltas can be added together
    Delta &operator+=(const Delta &rhs) {
        dx += rhs.dx;
        dy += rhs.dy;
        return *this;
    }

    // two Deltas can be subtracted
    Delta &operator-=(const Delta &rhs) {
        dx -= rhs.dx;
        dy -= rhs.dy;
        return *this;
    }

    // can be scaled by an integer
    Delta &operator*=(int rhs) {
        dx *= rhs;
        dy *= rhs;
        return *this;
    }
    Delta &operator/=(int rhs) {
        dx /= rhs;
        dy /= rhs;
        return *this;
    }
};
// this takes lhs by copy, so it doesn't modify the original lhs
inline Delta operator+(Delta lhs, const Delta &rhs) {
    lhs += rhs;
    return lhs;
}
inline Delta operator-(Delta lhs, const Delta &rhs) {
    lhs -= rhs;
    return lhs;
}
inline Delta operator*(Delta lhs, int rhs) {
    lhs *= rhs;
    return lhs;
}
inline Delta operator/(Delta lhs, int rhs) {
    lhs /= rhs;
    return lhs;
}

std::ostream &operator<<(std::ostream &os, const Delta &delta) {
    os << "Delta(" << delta.dx << ", " << delta.dy << ")";
    return os;
}

struct Pos {
    int x;
    int y;

    constexpr Pos() : x(0), y(0) {}
    constexpr Pos(int x, int y) : x(x), y(y) {}

    // we can add a Delta to a Pos, but not another Pos
    Pos &operator+=(const Delta &rhs) {
        x += rhs.dx;
        y += rhs.dy;
        return *this;
    }

    // can scale by an integer
    Pos &operator*=(int rhs) {
        x *= rhs;
        y *= rhs;
        return *this;
    }
    Pos &operator/=(int rhs) {
        x /= rhs;
        y /= rhs;
        return *this;
    }

    constexpr std::strong_ordering operator<=>(const Pos &) const = default;
};
// this takes lhs by copy, so it doesn't modify the original lhs
inline Pos operator+(Pos lhs, const Delta &rhs) {
    lhs += rhs;
    return lhs;
}
inline Pos operator*(Pos lhs, int rhs) {
    lhs *= rhs;
    return lhs;
}
inline Pos operator/(Pos lhs, int rhs) {
    lhs /= rhs;
    return lhs;
}
// can subtract two Pos, yielding a Delta
inline Delta operator-(const Pos &lhs, const Pos &rhs) {
    return {lhs.x - rhs.x, lhs.y - rhs.y};
}

std::ostream &operator<<(std::ostream &os, const Pos &pos) {
    os << "Pos(" << pos.x << ", " << pos.y << ")";
    return os;
}

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
    return ExpectInputHelper<std::remove_cvref_t<T>>(std::move(expected));
}
ExpectInputHelper<std::string> expect_input(const char *expected) {
    return ExpectInputHelper<std::string>{std::string{expected}};
}

// I/O manipulator that extracts an 8-bit number into a char instead of
// a single ASCII character, and inserts a char as an 8-bit number.
template <typename T>
class as_number {
    T &dest;
    friend std::istream &operator>>(std::istream &is, as_number h) {
        static_assert(!std::is_const_v<T>,
                      "Cannot extract into a const reference");
        constexpr bool is_char =
            std::is_same_v<T, char> || std::is_same_v<T, unsigned char>;
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
            std::is_same_v<std::remove_const_t<T>, char> ||
            std::is_same_v<std::remove_const_t<T>, unsigned char>;
        if constexpr (is_char) {
            using int_type =
                std::conditional_t<std::is_signed_v<T>, short, unsigned short>;
            os << static_cast<int_type>(h.dest);
        } else {
            os << h.dest;
        }
        return os;
    }

  public:
    explicit as_number(T &dest) : dest(dest) {}
};

/**
 * @brief  Parse command line arguments.
 * @return An istream for the specified input file.
 */
std::ifstream parse_args(int argc, char **argv) {
    if (argc != 2) {
        assert(argc >= 1);
        std::cout << "Usage: " << argv[0] << " <input file path>" << std::endl;
        std::exit(1);
    }
    return std::ifstream{argv[1]};
}

} // namespace aoc

template <>
struct std::hash<aoc::Pos> {
    std::size_t operator()(const aoc::Pos &pos) const noexcept {
        std::size_t h1 = std::hash<int>{}(pos.x);
        std::size_t h2 = std::hash<int>{}(pos.y);
        return h1 ^ (h2 << 1);
    }
};

// instantiate templates in an anonymous namespace, so static analyzers will
// check these functions
namespace {
[[maybe_unused]] void _lint_helper(std::istream &is) {
    is >> aoc::skip<char>(2);
    is >> aoc::skip<int>(1);
    is >> aoc::skip<std::string>(3);

    is >> aoc::expect_input('a');
    is >> aoc::expect_input("foo");
    is >> aoc::expect_input<std::string>("foo");
    is >> aoc::expect_input(std::string{"bar"});

    char ch;
    unsigned char uch;
    is >> aoc::as_number(ch);
    is >> aoc::as_number(uch);
}
} // namespace

#endif /* end of include guard: LIB_HPP_0IZKV7KG */
