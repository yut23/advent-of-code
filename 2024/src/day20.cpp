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
    if (args.input_type == aoc::InputType::EXAMPLE) {
        threshold = 50;
    } else {
        threshold = 100;
    }

    auto track = aoc::day20::Racetrack::read(args.infile);

    auto [part_1, part_2] = track.count_cheats(2, 20, threshold);
    std::cout << part_1 << "\n" << part_2 << "\n";

    return 0;
}
