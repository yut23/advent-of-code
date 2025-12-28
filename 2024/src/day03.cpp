/******************************************************************************
 * File:        day03.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-04
 *****************************************************************************/

#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout, cerr
#include <string>   // for string, stoi

#pragma GCC diagnostic push
#if defined(__GNUC__) && !defined(__clang__)
// GCC specific diagnostic
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
#include <regex> // for regex, smatch, sregex_iterator
#pragma GCC diagnostic pop

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv).infile;

    std::string memory = aoc::read_whole_stream(infile);

    std::regex op_regex(R"((mul)\((\d+),(\d+)\)|(do)\(\)|(don't)\(\))");
    std::sregex_iterator op_begin{memory.begin(), memory.end(), op_regex};
    std::sregex_iterator op_end{};

    int total_1 = 0, total_2 = 0;
    bool enabled = true;
    for (auto i = op_begin; i != op_end; ++i) {
        std::smatch match = *i;
        if constexpr (aoc::DEBUG) {
            std::cerr << "found match at " << match.position() << ": "
                      << match.str() << "\n";
        }
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
        // false positive in libstdc++ with GCC 13.2.1
        if (match.length(1) > 0) {
#pragma GCC diagnostic pop
            // mul
            int product = std::stoi(match[2].str()) * std::stoi(match[3].str());
            total_1 += product;
            if (enabled) {
                total_2 += product;
            }
        } else if (match.length(4) > 0) {
            // do
            enabled = true;
        } else if (match.length(5) > 0) {
            // don't
            enabled = false;
        }
    }

    std::cout << total_1 << "\n" << total_2 << "\n";

    return 0;
}
