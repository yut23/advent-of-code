/******************************************************************************
 * File:        day20.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-20
 *****************************************************************************/

#include "day20.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout, cerr

int main(int argc, char **argv) {
    auto args = aoc::parse_args(argc, argv);
    int threshold;
    if (args.filename.ends_with("example1.txt")) {
        threshold = 50;
    } else {
        threshold = 100;
    }

    auto track = aoc::day20::Racetrack::read(args.infile);

    // part 1
    auto cheats = track.find_cheats(2);
    std::cout << aoc::day20::count_cheats(cheats, threshold) << "\n";

    // part 2
    cheats = track.find_cheats(20);
    std::cout << aoc::day20::count_cheats(cheats, threshold) << "\n";

    return 0;
}
