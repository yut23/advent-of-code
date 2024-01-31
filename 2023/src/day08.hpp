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
#include <cstddef>       // for size_t
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
    const Node *left;
    const Node *right;

    Node(const std::string &id, const Node *left, const Node *right)
        : id(id), left(left), right(right) {}
};

struct Network {
    std::unordered_map<std::string, std::unique_ptr<Node>> nodes;

    Node *add_node(const std::string &id, const Node *left, const Node *right) {
        return nodes.try_emplace(id, std::make_unique<Node>(id, left, right))
            .first->second.get();
    }

    const Node *follow(const Node *node, char direction) const {
        return direction == 'L' ? node->left : node->right;
    }
    const Node *get(const std::string &id) const { return nodes.at(id).get(); }
};

Network read_maps(std::istream &is) {
    Network network;
    // find the pointer for a node id, creating a new node if it's not already
    // in the network
    const auto lookup_node = [&network](const std::string &id) {
        const auto it = network.nodes.find(id);
        if (it != network.nodes.end()) {
            return it->second.get();
        }
        return network.add_node(id, nullptr, nullptr);
    };
    std::string id, left, right;
    while (is >> id >> aoc::skip(1) >> left >> right) {
        left = left.substr(1, 3);
        right.resize(3);
        Node *node = lookup_node(id);
        node->left = lookup_node(left);
        node->right = lookup_node(right);
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
                                   const Network &network) {
    std::vector<const Node *> start_nodes;
    for (const auto &[id, node_ptr] : network.nodes) {
        if (id[2] == 'A') {
            start_nodes.push_back(node_ptr.get());
        }
    }
    std::vector<CycleInfo> cycles;
    for (auto node : start_nodes) {
        std::string start_id = node->id;
        std::map<std::pair<std::string, std::size_t>, FinishEntry> z_hits{};
        long step = 0;
        std::size_t dir_idx = 0;
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
