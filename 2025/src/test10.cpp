/******************************************************************************
 * File:        test10.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-12-24
 *****************************************************************************/

#include "day10.hpp"      // IWYU pragma: associated
#include "ds/grid.hpp"    // for Grid
#include "gauss_elim.hpp" // for RowPermuter
#include "lib.hpp"        // for Part
#include "subprocess.hpp" // for popen
#include <cstdlib>        // for size_t, EXIT_FAILURE, EXIT_SUCCESS
#include <fstream>        // for ifstream
#include <iostream>       // for ostream, cout, cerr
#include <string>         // for to_string
#include <vector>         // for vector

int main() {
    using namespace aoc::day10;

    std::ifstream infile{"input/day10/input.txt"};
    auto machines = read_input(infile);
    int line = 1;
    for (const Machine &machine : machines) {
        auto solver = Part2Solver(machine);
        auto orig_mtx = solver.mtx;
        auto orig_rhs = solver.rhs;
        solver.reduce_matrix();
        subprocess::popen cmd("python3",
                              {"src/test10_helper.py", std::to_string(line)},
                              std::cout, std::cerr);
        cmd.in() << orig_mtx.height << "\n";
        for (std::size_t row : solver.rp.row_order) {
            for (int col = 0; col < solver.mtx.width; ++col) {
                cmd.in() << orig_mtx.at(col, row) << ' ';
            }
            cmd.in() << orig_rhs.at(row) << "\n";
        }
        for (std::size_t row : solver.rp.row_order) {
            for (int col = 0; col < solver.mtx.width; ++col) {
                cmd.in() << solver.mtx.at(col, row) << ' ';
            }
            cmd.in() << solver.rhs.at(row) << "\n";
        }
        if (cmd.wait() != 0) {
            return EXIT_FAILURE;
        }
        std::cout << machine.min_presses<aoc::PART_2>() << "\n";
        ++line;
        if (line > 10) {
            break;
        }
    }

    return EXIT_SUCCESS;
}
