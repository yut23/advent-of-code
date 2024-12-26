/******************************************************************************
 * File:        day24.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-25
 *****************************************************************************/

#ifndef DAY24_HPP_SZBNX4LR
#define DAY24_HPP_SZBNX4LR

#include "graph_traversal.hpp"        // for topo_sort
#include "lib.hpp"                    // for skip, DEBUG, FAST
#include "unit_test/pretty_print.hpp" // for repr

#include <cassert>       // for assert
#include <cstddef>       // for size_t
#include <iostream>      // for istream
#include <string>        // for string, getline, to_string
#include <unordered_map> // for unordered_map
#include <unordered_set> // for unordered_set
#include <utility>       // for move

namespace aoc::day24 {

enum class GateOp { AND, OR, XOR };

struct Gate {
    std::string output;
    std::string input_1;
    std::string input_2;
    GateOp op;
};

std::istream &operator>>(std::istream &is, Gate &gate) {
    std::string tmp;
    is >> gate.input_1 >> tmp >> gate.input_2 >> aoc::skip(1) >> gate.output;
    if (tmp == "AND") {
        gate.op = GateOp::AND;
    } else if (tmp == "OR") {
        gate.op = GateOp::OR;
    } else if (tmp == "XOR") {
        gate.op = GateOp::XOR;
    } else if (is) {
        assert(false);
    }
    return is;
}

class LogicSim {
    std::unordered_map<std::string, bool> values;
    std::vector<Gate> gates;
    /// mapping from a gate's output to the corresponding index for that gate
    std::unordered_map<std::string, std::size_t> gate_lookup;
    /// mapping from a gate's output to gates that use it as an input
    std::unordered_map<std::string, std::unordered_set<std::string>>
        connections;

    void add_gate(Gate &&gate);
    bool eval_gate(const std::string &key) const;

  public:
    void evaluate();
    unsigned long z_value() const;
    static LogicSim read(std::istream &);
};

void LogicSim::add_gate(Gate &&gate) {
    gate_lookup.try_emplace(gate.output, gates.size());
    connections[gate.input_1].insert(gate.output);
    connections[gate.input_2].insert(gate.output);
    gates.push_back(std::move(gate));
}

bool LogicSim::eval_gate(const std::string &key) const {
    if constexpr (!aoc::FAST) {
        assert(!values.contains(key));
    }
    const Gate &gate = gates[gate_lookup.at(key)];
    bool input_0 = values.at(gate.input_1);
    bool input_1 = values.at(gate.input_2);
    switch (gate.op) {
    case GateOp::AND:
        return input_0 && input_1;
    case GateOp::OR:
        return input_0 || input_1;
    case GateOp::XOR:
        return input_0 != input_1;
    }
    assert(false);
}

void LogicSim::evaluate() {
    const auto process_neighbors = [this](const std::string &key,
                                          auto &&process) {
        if (key.empty()) {
            // source placeholder, process all nodes that have an initial value
            for (const auto &[u, _] : values) {
                process(u);
            }
        }
        if (auto it = connections.find(key); it != connections.end()) {
            for (const std::string &id : it->second) {
                process(gates[gate_lookup.at(id)].output);
            }
        }
    };

    const std::string source = "";
    std::vector<std::string> eval_order =
        aoc::graph::topo_sort(source, process_neighbors);
    if constexpr (aoc::DEBUG) {
        std::cerr << "eval_order: " << pretty_print::repr(eval_order) << "\n";
    }

    for (const auto &key : eval_order) {
        if (key.empty() || values.contains(key)) {
            continue;
        }
        values[key] = eval_gate(key);
    }
    if constexpr (aoc::DEBUG) {
        std::cerr << "values: " << pretty_print::repr(values) << "\n";
    }
}

unsigned long LogicSim::z_value() const {
    unsigned long z = 0;
    for (int i = 0; true; ++i) {
        std::string key = "z";
        if (i < 10) {
            key += '0';
        }
        key += std::to_string(i);
        if (auto it = values.find(key); it != values.end()) {
            if constexpr (aoc::DEBUG) {
                std::cerr << "value for " << key << ": " << it->second << "\n";
            }
            if (it->second) {
                z |= 1ul << i;
            }
        } else {
            break;
        }
    }
    return z;
}

LogicSim LogicSim::read(std::istream &is) {
    std::string line;
    LogicSim sim;
    while (std::getline(is, line)) {
        if (line.empty()) {
            break;
        }
        sim.values[line.substr(0, 3)] = line[5] == '1';
    }

    Gate gate{};
    while (is >> gate) {
        sim.add_gate(std::move(gate));
    }
    return sim;
}

} // namespace aoc::day24

#endif /* end of include guard: DAY24_HPP_SZBNX4LR */
