/******************************************************************************
 * File:        day22.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-22
 *****************************************************************************/

#include "day22.hpp"
#include "graph_traversal.hpp" // for topo_sort
#include "lib.hpp"             // for parse_args, DEBUG
#include <iostream>            // for cout, cerr
#include <unordered_set>       // for unordered_set

int part_1(const aoc::day22::BrickStack &stack) {
    int count = 0;
    for (const aoc::day22::Brick *brick : stack.bricks()) {
        bool is_safe = !stack.is_unsafe(brick);
        if (is_safe) {
            ++count;
        }
        if constexpr (aoc::DEBUG) {
            bool can_disintegrate = stack.can_disintegrate(brick);
            if (can_disintegrate != is_safe) {
                std::cerr << "mismatch for brick " << *brick
                          << ": can_disintegrate=" << can_disintegrate
                          << ", is_safe=" << is_safe << "\n";
                assert(can_disintegrate == is_safe);
            }
        }
    }
    return count;
}

int part_2(const aoc::day22::BrickStack &stack) {
    using namespace aoc::day22;

    int count = 0;

    for (const Brick *root : stack.bricks()) {
        if constexpr (aoc::DEBUG) {
            std::cerr << "resting on " << *root << ":\n";
            for (const auto tmp : stack.supporting.at(root)) {
                std::cerr << "  " << *tmp << "\n";
            }
        }

        const auto process_neighbors = [&stack](const Brick *brick,
                                                auto &&visit) {
            std::ranges::for_each(stack.supporting.at(brick), visit);
        };
        const auto ordering = aoc::graph::topo_sort(root, process_neighbors);

        std::unordered_set<const Brick *> falling{root};
        for (const Brick *brick : ordering) {
            bool can_fall = true;
            for (const Brick *b : stack.supported_by.at(brick)) {
                if (!falling.contains(b)) {
                    can_fall = false;
                    break;
                }
            }
            if (can_fall) {
                falling.insert(brick);
            }
        }
        falling.erase(root);
        count += falling.size();
    }

    return count;
}

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    auto stack = aoc::day22::BrickStack::read(infile);

    if constexpr (aoc::DEBUG) {
        std::cerr << "\npending bricks:\n";
        for (const auto &brick : stack.pending_bricks) {
            std::cerr << "  " << *brick << "\n";
        }
        std::cerr << "\n===============\n\n";
    }

    while (stack.settle_one()) {
        if constexpr (aoc::DEBUG) {
            std::cerr << "settled bricks:\n";
            for (const auto &[_, bricks] : stack.settled_bricks) {
                for (const auto &brick : bricks) {
                    std::cerr << "  " << *brick << "\n";
                }
            }
            std::cerr << "\npending bricks:\n";
            for (const auto &brick : stack.pending_bricks) {
                std::cerr << "  " << *brick << "\n";
            }
            std::cerr << "\n===============\n\n";
        }
    }

    if constexpr (aoc::DEBUG) {
        std::cerr << "settled bricks:\n";
        for (const auto &[_, bricks] : stack.settled_bricks) {
            for (const auto &brick : bricks) {
                std::cerr << "  " << *brick << "\n";
            }
        }
        std::cerr << "\nsafe to remove:\n";
        for (const auto &[_, bricks] : stack.settled_bricks) {
            for (const auto &brick : bricks) {
                if (!stack.is_unsafe(brick.get())) {
                    std::cerr << "  " << *brick << "\n";
                }
            }
        }
        std::cerr << "\n";
    }

    std::cout << part_1(stack) << "\n";
    std::cout << part_2(stack) << "\n";

    return 0;
}
