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

    long part_1 = 0;
    long part_2 = 0;
    for (const auto &code : codes) {
        // one numeric keypad that a robot is using
        std::vector<Key> keys = code.keys;
        // two directional keypads that robots are using
        long part_1_size = count_presses_memo(keys, 1 + 2);
        // 25 directional keypads that robots are using
        long part_2_size = count_presses_memo(keys, 1 + 25);
        long complexity_1 = part_1_size * code.numeric_value();
        part_1 += complexity_1;
        long complexity_2 = part_2_size * code.numeric_value();
        part_2 += complexity_2;
        if constexpr (aoc::DEBUG) {
            std::cerr << code.code << ": ";
            for (const auto key : keys) {
                std::cerr << key;
            }
            std::cerr << "\n  part 1 complexity = " << part_1_size << " * "
                      << code.numeric_value() << " = " << complexity_1 << "\n";
            std::cerr << "  part 2 complexity = " << part_2_size << " * "
                      << code.numeric_value() << " = " << complexity_2 << "\n";
        }
    }
    std::cout << part_1 << "\n" << part_2 << "\n";

    return 0;
}
