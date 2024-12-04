/******************************************************************************
 * File:        day20.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-21
 *****************************************************************************/

#include "day20.hpp"
#include "lib.hpp"  // for parse_args, DEBUG
#include <fstream>  // for ifstream
#include <iostream> // for cout
#include <string>   // for string

int part_1(aoc::day20::MessageBus &bus) {
    using namespace aoc::day20;
    bool debug = aoc::DEBUG;
    std::string button = "button";
    std::string broadcaster = "broadcaster";
    long presses = 0;
    while (presses < 1000) {
        bus.send_message(Message{button, broadcaster, MessageType::low});
        ++presses;
        while (bus.process(presses, debug))
            ;
        debug = false;
        bus.check_cycles(presses);
    }
    return bus.low_count() * bus.high_count();
}

long part_2(aoc::day20::MessageBus &bus) {
    if (!bus.has_rx()) {
        return -1;
    }
    using namespace aoc::day20;
    long presses = 1000;
    std::string button = "button";
    std::string broadcaster = "broadcaster";
    while (!bus.check_cycles(presses)) {
        bus.send_message(Message{button, broadcaster, MessageType::low});
        ++presses;
        while (bus.process(presses))
            ;
    }
    return bus.calculate_finish();
}

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    auto bus = aoc::day20::MessageBus::read_modules(infile);

    std::cout << part_1(bus) << "\n";
    std::cout << part_2(bus) << "\n";

    return 0;
}
