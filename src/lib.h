/******************************************************************************
 * File:        lib.h
 *
 * Author:      yut23
 * Created:     2022-12-01
 * Description: Common functions used in Advent of Code.
 *****************************************************************************/

#ifndef LIB_H_AT4RFPRV
#define LIB_H_AT4RFPRV

#include <cassert>
#include <fstream>
#include <iostream>

namespace aoc {

#ifdef DEBUG_MODE
[[maybe_unused]] constexpr bool DEBUG = true;
#else
constexpr bool DEBUG = false;
#endif

/**
 * @brief  Parse command line arguments.
 * @return An istream for the specified input file.
 */
std::ifstream parse_args(int argc, char **argv) {
    if (argc != 2) {
        assert(argc >= 1);
        std::cout << "Usage: " << argv[0] << " <input file path>" << std::endl;
        exit(EXIT_FAILURE);
    }
    return std::ifstream{argv[1]};
}

} // namespace aoc

#endif /* end of include guard: LIB_H_AT4RFPRV */
