/******************************************************************************
 * File:        day21.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-01-27
 *****************************************************************************/

#include "day21.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout

int main(int argc, char **argv) {
    auto args = aoc::parse_args(argc, argv);

    using namespace aoc::day21;
    auto codes = read_input(args.infile);

    long total = 0;
    for (const auto &code : codes) {
        // one numeric keypad that a robot is using
        std::vector<DirKey> keys = control_numeric(code.keys);
        // two directional keypads that robots are using
        for (int i = 0; i < 2; ++i) {
            keys = control_directional(keys);
        }
        long complexity = keys.size() * code.numeric_value();
        total += complexity;
        if constexpr (aoc::DEBUG) {
            std::cerr << code.code << ": ";
            for (const auto key : keys) {
                std::cerr << key;
            }
            std::cerr << "\n  complexity = " << keys.size() << " * "
                      << code.numeric_value() << " = " << complexity << "\n";
        }
    }
    std::cout << total << "\n";

    return 0;
}
