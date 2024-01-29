/******************************************************************************
 * File:        day08.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-08
 *****************************************************************************/

#ifndef DAY08_HPP_XKQTIHLN
#define DAY08_HPP_XKQTIHLN

#include "lib.hpp"       // for skip
#include <algorithm>     // for sort
#include <compare>       // for strong_ordering
#include <iostream>      // for istream, ostream
#include <map>           // for map
#include <memory>        // for unique_ptr, make_unique
#include <string>        // for string
#include <unordered_map> // for unordered_map
#include <utility>       // for pair, make_pair, move
#include <vector>        // for vector
// IWYU pragma: no_include <functional>  // for identity, less (ranges::sort)

namespace aoc::day08 {

struct Node {
    std::string id;
    std::string left;
    std::string right;

    Node(const std::string &id, const std::string &left,
         const std::string &right)
        : id(id), left(left), right(right) {}
};

struct Network {
    std::unordered_map<std::string, std::unique_ptr<Node>> nodes;

    void add_node(const std::string &id, const std::string &left,
                  const std::string &right) {
        nodes.try_emplace(id, std::make_unique<Node>(id, left, right));
    }

    const Node *follow(const Node *node, char direction) const {
        return nodes.at(direction == 'L' ? node->left : node->right).get();
    }
    const Node *get(const std::string &id) const { return nodes.at(id).get(); }
};

Network read_maps(std::istream &is) {
    Network network;
    std::string id, left, right;
    while (is >> id >> aoc::skip(1) >> left >> right) {
        left = left.substr(1, 3);
        right.resize(3);
        network.add_node(id, left, right);
    }
    return network;
}

struct FinishEntry {
    long step;
    std::string node_id;

    FinishEntry(long step, const std::string &node_id)
        : step(step), node_id(node_id) {}

    std::strong_ordering operator<=>(const FinishEntry &) const = default;
};

struct CycleInfo {
    std::string start_id;
    long start;
    long length;
    std::vector<FinishEntry> entries;

    CycleInfo(const std::string &start_id, long start, long length,
              const std::vector<FinishEntry> &entries)
        : start_id(start_id), start(start), length(length), entries(entries) {}
    CycleInfo(const std::string &start_id, long start, long length,
              std::vector<FinishEntry> &&entries)
        : start_id(start_id), start(start), length(length),
          entries(std::move(entries)) {}

    bool at_finish(long step) const {
        if (step < start) {
            // cppcheck-suppress useStlAlgorithm
            for (const FinishEntry &entry : entries) {
                if (step == entry.step) {
                    return true;
                }
            }
        } else {
            // cppcheck-suppress useStlAlgorithm // this is ~15% faster
            for (const FinishEntry &entry : entries) {
                if (entry.step - start >= 0 &&
                    (step - start) % length == entry.step - start) {
                    return true;
                }
            }
        }
        return false;
    }
};

std::ostream &operator<<(std::ostream &os, const CycleInfo &cycle) {
    os << "CycleInfo(start_id=" << cycle.start_id << ", start=" << cycle.start
       << ", length=" << cycle.length << ", entries={";
    for (auto it = cycle.entries.begin(); it != cycle.entries.end(); ++it) {
        if (it != cycle.entries.begin()) {
            os << ", ";
        }
        os << it->step << ": " << it->node_id;
    }
    os << "})";
    return os;
}

std::vector<CycleInfo> find_cycles(const std::string &directions,
                                   const aoc::day08::Network &network) {
    std::vector<const aoc::day08::Node *> curr_nodes;
    for (const auto &[id, node_ptr] : network.nodes) {
        if (id[2] == 'A') {
            curr_nodes.push_back(node_ptr.get());
        }
    }
    std::vector<aoc::day08::CycleInfo> cycles;
    for (auto node : curr_nodes) {
        std::string start_id = node->id;
        std::map<std::pair<std::string, int>, FinishEntry> z_hits{};
        long step = 0;
        int dir_idx = 0;
        while (true) {
            if (node->id[2] == 'Z') {
                auto key = std::make_pair(node->id, dir_idx);
                auto it = z_hits.find(key);
                if (it != z_hits.end()) {
                    // found a cycle!
                    int start = it->second.step;
                    std::vector<FinishEntry> entries;
                    for (auto &[_, entry] : z_hits) {
                        entries.push_back(std::move(entry));
                    }
                    std::ranges::sort(entries);
                    cycles.emplace_back(start_id, start, step - start, entries);
                    break;
                } else {
                    z_hits.try_emplace(key, FinishEntry{step, node->id});
                }
            }
            node = network.follow(node, directions[dir_idx]);
            // repeat the direction string if we hit the end
            dir_idx = (dir_idx + 1) % directions.size();
            ++step;
        }
    }
    return cycles;
}

} // namespace aoc::day08

#endif /* end of include guard: DAY08_HPP_XKQTIHLN */
