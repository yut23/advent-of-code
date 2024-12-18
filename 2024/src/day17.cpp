/******************************************************************************
 * File:        day17.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-17
 *****************************************************************************/

#include "day17.hpp"
#include "lib.hpp"
#include <fstream>  // for ifstream
#include <iostream> // for cout

int main(int argc, char **argv) {
    auto args = aoc::parse_args(argc, argv);

    using namespace aoc::day17;
    auto [program, regs] = read_input(args.infile);

    // part 1
    std::ostringstream oss{};
    run_program(program, regs,
                [&oss](code_t value) { oss << ',' << aoc::as_number(value); });
    std::cout << oss.str().substr(1) << "\n";
    if constexpr (aoc::DEBUG) {
        std::cerr << "\n";
    }

    // part 2
    if (auto reason = is_possible_quine(program)) {
        std::cerr << "skipping part 2: " << *reason << "\n";
        return 0;
    }
    std::uint64_t quine_input = solve_quine(program);
    if constexpr (aoc::DEBUG) {
        std::cerr << "checking for quine with A=" << quine_input << "...\n";
        std::vector<code_t> output;
        regs.A = quine_input;
        run_program(program, regs,
                    [&output](code_t value) { output.push_back(value); });
        assert(output == program);
    }
    std::cout << quine_input << "\n";

    return 0;
}
