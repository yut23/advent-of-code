/******************************************************************************
 * File:        lib.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2022-12-01
 * Description: Common functions used in Advent of Code.
 *****************************************************************************/

#ifndef LIB_HPP_0IZKV7KG
#define LIB_HPP_0IZKV7KG

#include <algorithm>   // for max
#include <cassert>     // for assert
#include <compare>     // for strong_ordering
#include <cstdlib>     // for abs, exit
#include <fstream>     // for ifstream  // IWYU pragma: keep
#include <iostream>    // for cout
#include <string>      // for string
#include <type_traits> // for is_same_v, is_signed_v, conditional_t

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

enum class Direction : char { up = 'U', down = 'D', left = 'L', right = 'R' };

Direction opposite(Direction dir) {
    switch (dir) {
    case Direction::up:
        return Direction::down;
    case Direction::down:
        return Direction::up;
    case Direction::left:
        return Direction::right;
    case Direction::right:
        return Direction::left;
    }
    assert(false);
}

std::istream &operator>>(std::istream &is, Direction &dir) {
    char ch = 0;
    if (is >> ch) {
        dir = static_cast<Direction>(ch);
    }
    return is;
}
std::ostream &operator<<(std::ostream &os, const Direction &dir) {
    os << static_cast<char>(dir);
    return os;
}

struct Delta {
    int dx;
    int dy;

    constexpr Delta(int dx, int dy) : dx(dx), dy(dy) {}
    explicit constexpr Delta(Direction dir, bool invert_vertical = false)
        : dx(0), dy(0) {
        switch (dir) {
        case Direction::up:
            dy = 1;
            break;
        case Direction::down:
            dy = -1;
            break;
        case Direction::right:
            dx = 1;
            break;
        case Direction::left:
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

// use an anonymous namespace to hide this implementation detail
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
} // namespace

template <typename T = std::string>
SkipInputHelper<T> skip(int count = 1) {
    return SkipInputHelper<T>{count};
}

// I/O manipulator that reads an 8-bit number into a char instead of a single
// ASCII character.
template <typename T>
class as_number {
    T &dest;
    friend std::istream &operator>>(std::istream &is, as_number h) {
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

#endif /* end of include guard: LIB_HPP_0IZKV7KG */
