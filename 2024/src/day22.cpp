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
    auto args = aoc::parse_args(argc, argv);

    std::uint64_t part_1 = 0;
    std::uint32_t secret;
    for (int monkey_number = 0; args.infile >> secret; ++monkey_number) {
        aoc::day22::PriceSequence sequence{secret, monkey_number};
        for (int i = 0; i < 2000; ++i) {
            sequence.evolve(i);
        }
        part_1 += sequence.get_secret();
    }
    std::cout << part_1 << "\n";
    int part_2 = aoc::day22::PriceSequence::find_best_sell_sequence();
    std::cout << part_2 << "\n";
    if (args.input_type == aoc::InputType::MAIN && part_2 >= 1450) {
        std::cerr << "\033[1;31mERROR:\033[0m result for part 2 is too high\n";
        return 1;
    }

    return 0;
}
