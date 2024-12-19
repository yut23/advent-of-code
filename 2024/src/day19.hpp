/******************************************************************************
 * File:        day19.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-19
 *****************************************************************************/

#ifndef DAY19_HPP_CSTWRABP
#define DAY19_HPP_CSTWRABP

#include <array>    // for array
#include <cassert>  // for assert
#include <cstddef>  // for size_t
#include <cstdint>  // for uint8_t
#include <iostream> // for istream, ws
#include <list>     // for list
#include <optional> // for optional
#include <sstream>  // for istringstream
#include <string>   // for basic_string, string, getline
#include <utility>  // for pair, move
#include <vector>   // for vector

namespace aoc::day19 {

enum class Color : std::uint8_t {
    white = 0, // w
    blue = 1,  // u
    black = 2, // b
    red = 3,   // r
    green = 4, // g
};

using ColorString = std::basic_string<Color>;

std::basic_string<Color> str_to_colors(const std::string &s) {
    std::basic_string<Color> vec;
    for (char ch : s) {
        switch (ch) {
        case 'w':
            vec.push_back(Color::white);
            break;
        case 'u':
            vec.push_back(Color::blue);
            break;
        case 'b':
            vec.push_back(Color::black);
            break;
        case 'r':
            vec.push_back(Color::red);
            break;
        case 'g':
            vec.push_back(Color::green);
            break;
        default:
            assert(false);
            break;
        }
    }
    return vec;
}

// uses a trie to check designs against towel patterns
class DesignChecker {
    struct Node {
        bool is_terminal = false;
        std::array<Node *, 5> children{};

        Node *&at(Color color) {
            return children[static_cast<std::uint8_t>(color)];
        }
        const Node *at(Color color) const {
            return children[static_cast<std::uint8_t>(color)];
        }
    };
    std::list<Node> nodes;

    /// gets a child node, creating it if necessary
    Node &get_child(Node &parent, Color index);
    Node *root_ptr() { return &nodes.front(); }
    const Node *root_ptr() const { return &nodes.front(); }

    long check_helper(const ColorString &design,
                      std::vector<std::optional<long>> &memo,
                      std::size_t start) const;

  public:
    DesignChecker() : nodes(1) {}

    void add_pattern(const ColorString &pattern);
    long check(const ColorString &design) const;
};

DesignChecker::Node &DesignChecker::get_child(DesignChecker::Node &parent,
                                              Color color) {
    Node *&child_ptr = parent.at(color);
    if (child_ptr == nullptr) {
        child_ptr = &nodes.emplace_back();
    }
    return *child_ptr;
}

void DesignChecker::add_pattern(const ColorString &pattern) {
    Node *node = root_ptr();
    for (auto it = pattern.begin(); it != pattern.end(); ++it) {
        node = &get_child(*node, *it);
    }
    node->is_terminal = true;
}

long DesignChecker::check(const ColorString &design) const {
    std::vector<std::optional<long>> memo(design.size());
    return check_helper(design, memo, 0);
}

// check a design against all possible towel combinations by recursing when
// reaching a terminal node
long DesignChecker::check_helper(const ColorString &design,
                                 std::vector<std::optional<long>> &memo,
                                 std::size_t start) const {
    // base case
    if (start == design.size()) {
        return true;
    }
    if (auto cached = memo[start]) {
        return *cached;
    }
    const Node *node = root_ptr();
    long count = 0;
    for (std::size_t i = start; i < design.size(); ++i) {
        node = node->at(design[i]);
        if (node == nullptr) {
            break;
        }
        if (node->is_terminal) {
            count += check_helper(design, memo, i + 1);
        }
    }
    memo[start] = count;
    return count;
}

std::pair<DesignChecker, std::vector<ColorString>>
read_input(std::istream &is) {
    std::string tmp;
    std::getline(is, tmp);
    // add trailing comma for simpler parsing
    std::istringstream iss{tmp + ","};
    DesignChecker checker;
    while (iss >> tmp) {
        // remove trailing comma
        tmp.pop_back();
        checker.add_pattern(str_to_colors(tmp));
    }
    // consume blank line
    is >> std::ws;

    std::vector<std::basic_string<Color>> designs;
    while (is >> tmp) {
        designs.push_back(str_to_colors(tmp));
    }
    return {std::move(checker), std::move(designs)};
}

} // namespace aoc::day19

#endif /* end of include guard: DAY19_HPP_CSTWRABP */
