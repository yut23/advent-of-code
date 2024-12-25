/******************************************************************************
 * File:        day22.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-22
 *****************************************************************************/

#include "day22.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv).infile;

    long total = 0;
    long secret;
    while (infile >> secret) {
        for (int i = 0; i < 2000; ++i) {
            aoc::day22::evolve_secret(secret);
        }
        total += secret;
    }
    std::cout << total << "\n";

    return 0;
}
