/******************************************************************************
 * File:        day02.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-03
 *****************************************************************************/

#include "lib.hpp"
#include <iostream> // for cout
#include <map>      // for map
#include <optional> // for optional
#include <sstream>  // for istringstream
#include <string>   // for string, getline

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    int total = 0;
    std::string line;
    while (std::getline(infile, line)) {
        int game_id;
        std::istringstream ss{line};
        // read "Game <ID>: "
        ss >> aoc::skip(1) >> game_id >> aoc::skip<char>(1);
        int count;
        std::string color;
        std::optional<char> sep = ';';
        bool valid = true;
        if constexpr (aoc::DEBUG) {
            std::cerr << "game " << game_id << "\n";
        }
        for (int i = 1; sep; ++i) {
            std::map<std::string, int> counts;
            if constexpr (aoc::DEBUG) {
                std::cerr << " set " << i << "\n";
            }
            while (ss) {
                // read "<count> <color>[;,]"
                ss >> count >> color;
                sep = color.back();
                if (*sep == ';' || *sep == ',') {
                    color.pop_back();
                } else {
                    sep = {};
                }
                if constexpr (aoc::DEBUG) {
                    std::cerr << "  " << count << " " << color << " "
                              << "\n";
                }
                counts[color] = count;
                if (*sep == ';') {
                    break;
                }
            }
            if constexpr (aoc::DEBUG) {
                std::cerr << " counts: "
                          << "red=" << counts["red"]
                          << " green=" << counts["green"]
                          << " blue=" << counts["blue"] << '\n';
            }
            if (counts["red"] > 12 || counts["green"] > 13 ||
                counts["blue"] > 14) {
                valid = false;
                break;
            }
        }
        if (valid) {
            total += game_id;
        }
    }

    std::cout << total << "\n";
    return 0;
}
