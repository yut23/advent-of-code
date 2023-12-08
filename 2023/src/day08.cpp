/******************************************************************************
 * File:        day08.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-08
 *****************************************************************************/

#include "lib.hpp"
#include <iostream> // for cout
#include <map>      // for map
#include <memory>   // for unique_ptr, make_unique
#include <string>   // for string

namespace aoc::day08 {
struct Node {
    std::string id;
    std::string left;
    std::string right;

    Node(const std::string &id, const std::string &left,
         const std::string &right)
        : id(id), left(left), right(right) {}
};

using Network = std::map<std::string, std::unique_ptr<Node>>;

Network read_maps(std::istream &is) {
    Network network;
    std::string id, left, right;
    while (is >> id >> aoc::skip(1) >> left >> right) {
        left = left.substr(1, 3);
        right.resize(3);
        network.try_emplace(id, std::make_unique<Node>(id, left, right));
    }
    return network;
}
} // namespace aoc::day08

int part_1(const std::string &directions, const aoc::day08::Network &network) {
    int steps = 0;
    aoc::day08::Node *curr = network.find("AAA")->second.get();
    auto dir_it = directions.begin();
    while (curr->id != "ZZZ") {
        curr = network.find(*dir_it == 'L' ? curr->left : curr->right)
                   ->second.get();
        // repeat the direction string if we hit the end
        if (++dir_it == directions.end()) {
            dir_it = directions.begin();
        }
        ++steps;
    }
    return steps;
}

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    std::string directions;
    infile >> directions;
    auto network = aoc::day08::read_maps(infile);

    std::cout << part_1(directions, network) << "\n";

    return 0;
}
