/******************************************************************************
 * File:        day22.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-22
 *****************************************************************************/

#include "day22.hpp"
#include "lib.hpp"  // for parse_args, DEBUG
#include <iostream> // for cout, cerr

int part_1(aoc::day22::BrickStack &stack) {
    int count = 0;
    for (const auto &[_, brick] : stack.settled_bricks) {
        bool is_safe = !stack.unsafe.contains(brick.get());
        if (is_safe) {
            ++count;
        }
        if constexpr (aoc::DEBUG) {
            bool can_disintegrate = stack.can_disintegrate(brick.get());
            if (can_disintegrate != is_safe) {
                std::cerr << "mismatch for brick " << *brick
                          << ": can_disintegrate=" << can_disintegrate
                          << ", is_safe=" << is_safe << "\n";
                {
                    const auto range =
                        stack.settled_bricks.equal_range(brick->p2.z);
                    std::cerr << "other blocks at same height:\n";
                    for (auto it = range.first; it != range.second; ++it) {
                        std::cerr << "  " << *it->second << "\n";
                    }
                }
                if (!is_safe) {
                    const auto range = stack.unsafe.equal_range(brick.get());
                    std::cerr << "\nresting blocks:\n";
                    for (auto it = range.first; it != range.second; ++it) {
                        std::cerr << "  " << *it->second << "\n";
                    }
                }
                assert(can_disintegrate == is_safe);
            }
        }
    }
    return count;
}

int part_2(aoc::day22::BrickStack &stack) {
    using namespace aoc::day22;

    std::map<const Brick *, int> counts;

    auto helper = [&stack, &counts](const Brick *brick, const auto rec) {
        {
            const auto it = counts.find(brick);
            if (it != counts.end()) {
                return it->second;
            }
        }
        int count = 0;
        const auto range = stack.unsafe.equal_range(brick);
        for (auto it = range.first; it != range.second; ++it) {
            count += 1 + rec(it->second, rec);
        }
        return count;
    };

    int count = 0;
    for (const auto &[_, brick] : stack.settled_bricks) {
        count += helper(brick.get(), helper);
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
            for (const auto &[_, brick] : stack.settled_bricks) {
                std::cerr << "  " << *brick << "\n";
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

    std::cout << part_1(stack) << "\n";
    // std::cout << part_2(stack) << "\n";

    return 0;
}
