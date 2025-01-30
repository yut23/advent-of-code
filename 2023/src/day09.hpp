/******************************************************************************
 * File:        day09.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-09
 *****************************************************************************/

#ifndef DAY09_HPP_UZDLAO3M
#define DAY09_HPP_UZDLAO3M

#include "lib.hpp" // for read_vector
#include <cassert> // for assert
#include <cstddef> // for size_t
#include <sstream> // for istringstream
#include <string>  // for string
#include <vector>  // for vector

namespace aoc::day09 {

std::vector<int> read_history(const std::string &line) {
    std::istringstream ss{line};
    return aoc::read_vector<int>(ss);
}

std::vector<int> diff(const std::vector<int> &vec) {
    std::vector<int> diff_vec;
    assert(vec.size() > 1);
    for (std::size_t i = 0; i < vec.size() - 1; ++i) {
        diff_vec.push_back(vec[i + 1] - vec[i]);
    }
    return diff_vec;
}

} // namespace aoc::day09

#endif /* end of include guard: DAY09_HPP_UZDLAO3M */
