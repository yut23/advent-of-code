/******************************************************************************
 * File:        day20.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-21
 *****************************************************************************/

#include "day20.hpp"
#include "lib.hpp"  // for parse_args, DEBUG
#include <iostream> // for cout

int part_1(aoc::day20::MessageBus &bus) {
    bool debug = aoc::DEBUG;
    for (int i = 0; i < 1000; ++i) {
        using namespace aoc::day20;
        bus.send_message(Message{"button", "broadcaster", MessageType::low});
        while (bus.process(debug))
            ;
        debug = false;
    }
    return bus.get_low_count() * bus.get_high_count();
}

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    auto bus = aoc::day20::MessageBus::read_modules(infile);

    std::cout << part_1(bus) << "\n";

    return 0;
}
