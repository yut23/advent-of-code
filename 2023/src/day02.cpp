/******************************************************************************
 * File:        day02.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-03
 *****************************************************************************/

#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout
#include <map>      // for map
#include <optional> // for optional
#include <sstream>  // for istringstream
#include <string>   // for string, getline

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv).infile;

    int part_1 = 0, part_2 = 0;
    std::string line;
    while (std::getline(infile, line)) {
        int game_id;
        std::istringstream ss{line};
        // read "Game <ID>: "
        ss >> aoc::skip(1) >> game_id >> aoc::skip<char>(1);
        int count;
        std::string color;
        std::optional<char> sep = ';';
        if constexpr (aoc::DEBUG) {
            std::cerr << "game " << game_id << "\n";
        }
        std::map<std::string, int> max_counts;
        bool valid = true;
        for (int i = 0; sep; ++i) {
            std::map<std::string, int> counts;
            if constexpr (aoc::DEBUG) {
                std::cerr << " set " << i << "\n";
            }
            while (ss) {
                // read "<count> <color>[;,]?"
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
                if (count > max_counts[color]) {
                    max_counts[color] = count;
                }
                if (sep && *sep == ';') {
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
            }
        }
        if (valid) {
            part_1 += game_id;
        }
        part_2 += max_counts["red"] * max_counts["green"] * max_counts["blue"];
    }

    std::cout << part_1 << "\n";
    std::cout << part_2 << "\n";
    return 0;
}
