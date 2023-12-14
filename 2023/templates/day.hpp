/******************************************************************************
 * File:        day{{DAY}}.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     {{DATE}}
 *****************************************************************************/

#ifndef DAY_HPP_{{GUARD_NONCE}}
#define DAY_HPP_{{GUARD_NONCE}}

#include "lib.hpp"
#include <iostream> // for istream
#include <string>   // for string, getline

namespace aoc::day{{DAY}} {

auto read_input(std::istream &is) {
    // read file line-by-line
    std::string line;
    while (std::getline(infile, line)) {
        // do stuff
    }
    return;
}

} // namespace aoc::day{{DAY}}

#endif /* end of include guard: DAY_HPP_{{GUARD_NONCE}} */
