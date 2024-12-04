/******************************************************************************
 * File:        day03.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-04
 *****************************************************************************/

#ifndef DAY03_HPP_V6YG8RFJ
#define DAY03_HPP_V6YG8RFJ

#include <iostream> // for istream
#include <iterator> // for istreambuf_iterator
#include <string>   // for string

namespace aoc::day03 {

auto read_input(std::istream &is) {
    // read entire file into a string
    std::string contents(std::istreambuf_iterator<char>(is), {});
    return contents;
}

} // namespace aoc::day03

#endif /* end of include guard: DAY03_HPP_V6YG8RFJ */
