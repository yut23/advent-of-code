/******************************************************************************
 * File:        day{{DAY}}.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     {{DATE}}
 *****************************************************************************/

#ifndef DAY{{DAY}}_HPP_{{GUARD_NONCE}}
#define DAY{{DAY}}_HPP_{{GUARD_NONCE}}

#include <iostream> // for istream
#include <string>   // for string, getline

namespace aoc::day{{DAY}} {

auto read_input(std::istream &is) {
    // read file line-by-line
    std::string line;
    while (std::getline(is, line)) {
        // do stuff
    }
    return line;
}

} // namespace aoc::day{{DAY}}

#endif /* end of include guard: DAY{{DAY}}_HPP_{{GUARD_NONCE}} */
