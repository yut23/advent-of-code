/******************************************************************************
 * File:        day24.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-25
 *****************************************************************************/

#include "day24.hpp"
#include "lib.hpp"
#include <array>    // for array
#include <fstream>  // for ifstream
#include <iostream> // for cout, cerr, dec, hex, showbase

int main(int argc, char **argv) {
    auto args = aoc::parse_args(argc, argv);

    using namespace aoc::day24;
    auto [sim, x, y] = LogicSim::read(args.infile);
    sim.evaluate(x, y);

    std::cout << sim.z_value() << "\n";

    if (args.input_type != aoc::InputType::MAIN) {
        return 0;
    }

    // part 2
    // check addition for errors 1 bit at a time
    const auto check_addition = [&sim = sim](std::uint64_t x, std::uint64_t y,
                                             bool mark) {
        std::uint64_t expected = x + y;
        sim.evaluate(x, y);
        std::uint64_t z = sim.z_value();
        if (z != expected) {
            if (mark) {
                std::vector<int> bad_bits;
                if constexpr (aoc::DEBUG) {
                    std::cerr << "error in bit ";
                }
                for (int i = 0; i <= sim.num_bits; ++i) {
                    if ((z ^ expected) & (1ull << i)) {
                        bad_bits.push_back(i);
                        if constexpr (aoc::DEBUG) {
                            std::cerr << i << ", ";
                        }
                    }
                }
                if constexpr (aoc::DEBUG) {
                    std::cerr << "calculating " << std::hex << std::showbase
                              << x << " + " << y << ": got " << z
                              << ", expected " << expected << std::dec << "\n";
                }
                // mark the first incorrect bit and the bit after that as
                // potentially swapped
                sim.mark(bad_bits[0], CheckStatus::MAYBE_SWAPPED);
                if (bad_bits[0] + 1 <= sim.num_bits) {
                    sim.mark(bad_bits[0] + 1, CheckStatus::MAYBE_SWAPPED);
                }
            }
            return false;
        }
        return true;
    };

    // check entire truth table for the adder for bit i
    const auto check_bit = [&sim = sim, &check_addition](int i,
                                                         bool mark = false) {
        if (i >= sim.num_bits) {
            // the most-significant bit was tested as the carry out from the
            // previous bit
            return true;
        }
        std::vector<std::array<std::uint64_t, 2>> half_adder_inputs = {
            {0, 0},
            {0, 1ull << i},
            {1ull << i, 0},
            {1ull << i, 1ull << i},
        };
        // test each half adder input without and with a carry in, if applicable
        std::vector<std::array<std::uint64_t, 2>> carry_inputs{0};
        if (i > 0) {
            // skip carry in for least-significant bit
            carry_inputs.push_back({1ull << (i - 1), 1ull << (i - 1)});
        }

        std::uint64_t x, y;
        for (const auto &carry_input : carry_inputs) {
            for (const auto &half_adder_input : half_adder_inputs) {
                x = carry_input[0] | half_adder_input[0];
                y = carry_input[1] | half_adder_input[1];
                if (!check_addition(x, y, mark)) {
                    return false;
                }
            }
        }
        return true;
    };

    for (int i = 0; i < sim.num_bits; ++i) {
        if (!check_bit(i, true)) {
            // try swapping all valid pairs of potentially swapped gates
            auto swaps = sim.get_swaps();
            bool found_swap = false;
            for (const auto &swap_hdl : swaps) {
                if constexpr (aoc::DEBUG) {
                    std::cerr << "trying swap: ";
                    sim.print_swap(std::cerr, swap_hdl);
                    std::cerr << "\n";
                }
                sim.swap_outputs(swap_hdl);
                bool valid = true;
                for (int j = i; j <= i + 2; ++j) {
                    if (!check_bit(j, false)) {
                        valid = false;
                        break;
                    }
                }
                if (valid) {
                    found_swap = true;
                    break;
                } else {
                    // undo this swap and try the next one
                    sim.unswap_outputs(swap_hdl);
                }
            }
            if (found_swap) {
                if constexpr (aoc::DEBUG) {
                    std::cerr << "swap successful!\n\n";
                }
                sim.mark(i, CheckStatus::GOOD);
            } else {
                assert("no swap found!" && false);
            }
            // retry the current bit
            //--i;
        } else {
            // mark all inputs as good
            sim.mark(i, CheckStatus::GOOD);
        }
    }

    std::cout << sim.format_swapped_gates() << "\n";

    return 0;
}
