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
#include <string>   // for string

int main(int argc, char **argv) {
    using namespace aoc::day09;
    std::ifstream infile = aoc::parse_args(argc, argv);
    // NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
    const bool is_example =
        std::string{argv[1]}.find("example") != std::string::npos;

    auto disk_layout = DiskLayout::read(infile);
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
