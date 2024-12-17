/******************************************************************************
 * File:        day09.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-09
 *****************************************************************************/

#include "day09.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout, cerr

int main(int argc, char **argv) {
    using namespace aoc::day09;
    auto args = aoc::parse_args(argc, argv);
    const bool is_example = args.input_type == aoc::InputType::EXAMPLE;

    auto disk_layout = DiskLayout::read(args.infile);
    if constexpr (aoc::DEBUG) {
        if (is_example) {
            std::cerr << "initial:    " << disk_layout << "\n";
            std::cerr << disk_layout.calculate_checksum() << "\n";
        }
    }

    DiskLayout compacted = disk_layout.compact_fragmented();
    if constexpr (aoc::DEBUG) {
        if (is_example) {
            std::cerr << "fragmented: " << compacted << "\n";
        }
    }
    std::cout << compacted.calculate_checksum() << "\n";

    compacted = disk_layout.compact_smart();
    if constexpr (aoc::DEBUG) {
        if (is_example) {
            std::cerr << "smart:      " << compacted << "\n";
        }
    }
    std::cout << compacted.calculate_checksum() << "\n";

    return 0;
}
