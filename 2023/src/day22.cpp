/******************************************************************************
 * File:        day22.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-22
 *****************************************************************************/

#include "day22.hpp"
#include "lib.hpp"  // for parse_args, DEBUG
#include <iostream> // for cout, cerr

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    auto stack = aoc::day22::BrickStack::read(infile);

    do {
        if constexpr (aoc::DEBUG) {
            std::cerr << "settled bricks:\n";
            for (const auto &[_, brick] : stack.settled_bricks) {
                std::cerr << "  " << *brick << "\n";
            }
            std::cerr << "\npending bricks:\n";
            for (const auto &brick : stack.pending_bricks) {
                std::cerr << "  " << *brick << "\n";
            }
            std::cerr << "\n===============\n\n";
        }
    } while (stack.settle_one());

    if constexpr (aoc::DEBUG) {
        std::cerr << "settled bricks:\n";
        for (const auto &[_, brick] : stack.settled_bricks) {
            std::cerr << "  " << *brick << "\n";
        }
        std::cerr << "\nsafe to remove:\n";
        for (const auto &[_, brick] : stack.settled_bricks) {
            if (!stack.unsafe.contains(brick.get())) {
                std::cerr << "  " << *brick << "\n";
            }
        }
        std::cerr << "\n";
    }

    std::cout << stack.settled_bricks.size() - stack.unsafe.size() << "\n";

    return 0;
}
