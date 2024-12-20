/******************************************************************************
 * File:        day20.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-20
 *****************************************************************************/

#include "day20.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iomanip>  // for setw
#include <iostream> // for cout, cerr

int main(int argc, char **argv) {
    auto args = aoc::parse_args(argc, argv);
    int threshold;
    if (args.filename.ends_with("example1.txt")) {
        threshold = 11;
    } else {
        threshold = 100;
    }

    auto track = aoc::day20::Racetrack::read(args.infile);

    auto cheats = track.find_cheats();
    int part_1 = 0;
    for (const auto &cheat : cheats) {
        if constexpr (aoc::DEBUG) {
            std::cerr << "cheat saves " << std::setw(3) << cheat.time_saved
                      << " picoseconds: " << cheat.start << " to " << cheat.end
                      << "\n";
        }
        if (cheat.time_saved >= threshold) {
            ++part_1;
        }
    }
    std::cout << part_1 << "\n";

    return 0;
}
