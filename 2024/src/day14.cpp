/******************************************************************************
 * File:        day14.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-14
 *****************************************************************************/

#include "day14.hpp"
#include "lib.hpp"
#include "util/math.hpp" // for CRT
#include <fstream>       // for ifstream
#include <iostream>      // for cout
#include <limits>        // for numeric_limits

int main(int argc, char **argv) {
    auto args = aoc::parse_args(argc, argv);
    const bool is_example = args.input_type == aoc::InputType::EXAMPLE;
    aoc::Pos bounds = is_example ? aoc::Pos{11, 7} : aoc::Pos{101, 103};

    auto robots = aoc::day14::Robots::read(args.infile, bounds);

    struct min_entry {
        double value = std::numeric_limits<double>::max();
        int time = -1;

        void update(double new_value, int new_time) {
            if (new_value < value) {
                value = new_value;
                time = new_time;
            }
        }
    };

    min_entry min_x_entropy, min_y_entropy;
    int stop_time = std::max(100, std::max(bounds.x, bounds.y) - 1);
    for (int time = 0; time <= stop_time; ++time) {
        if (time == 100) {
            // part 1
            std::cout << robots.safety_factor() << "\n";
        }

        const auto [x_entropy, y_entropy] = robots.calc_entropy();
        min_x_entropy.update(x_entropy, time);
        min_y_entropy.update(y_entropy, time);
        if constexpr (aoc::DEBUG) {
            std::cerr << "t=" << time << ": H(X) = " << x_entropy
                      << ", H(Y) = " << y_entropy << "\n";
        }

        robots.update();
    }

    if (!is_example) {
        if constexpr (aoc::DEBUG) {
            std::cerr << "min x entropy at t=" << min_x_entropy.time
                      << ", H(X) = " << min_x_entropy.value << "\n";
            std::cerr << "min y entropy at t=" << min_y_entropy.time
                      << ", H(Y) = " << min_y_entropy.value << "\n";
        }

        // calculate the easter egg time using CRT
        aoc::math::CRT crt;
        crt.add_entry(min_x_entropy.time, bounds.x);
        crt.add_entry(min_y_entropy.time, bounds.y);
        std::cout << crt.solve().remainder << "\n";
    }

    return 0;
}
