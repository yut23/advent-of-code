/******************************************************************************
 * File:        day11.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-12-11
 *****************************************************************************/

#include "day11.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv).infile;
    using aoc::day11::DeviceMap;

    const auto device_map = DeviceMap::read(infile);

    const char *source = "you";
    if (!device_map.contains(source)) {
        source = "svr";
    }
    long part1 = device_map.count_paths(source, "out");
    std::cout << part1 << "\n";

    if (device_map.contains("svr")) {
        long part2 = 0;
        // find paths that go from svr -> {dac, fft} -> {fft, dac} -> out
        part2 += device_map.count_paths("svr", "dac") *
                 device_map.count_paths("dac", "fft") *
                 device_map.count_paths("fft", "out");
        part2 += device_map.count_paths("svr", "fft") *
                 device_map.count_paths("fft", "dac") *
                 device_map.count_paths("dac", "out");
        std::cout << part2 << "\n";
    }

    return 0;
}
