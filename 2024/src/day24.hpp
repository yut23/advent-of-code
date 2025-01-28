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

#include <algorithm>        // for sort
#include <cassert>          // for assert
#include <cstddef>          // for size_t
#include <cstdint>          // for uint64_t
#include <functional>       // for hash (unordered_map, unordered_set)
#include <initializer_list> // for initializer_list
#include <iostream>         // for istream, ostream, cerr
#include <set>              // for set
#include <sstream>          // for ostringstream
#include <string>           // for string, getline, to_string
#include <tuple>            // for tuple
#include <unordered_map>    // for unordered_map
#include <unordered_set>    // for unordered_set
#include <utility>          // for move
#include <vector>           // for vector

namespace aoc::day24 {

enum class GateOp { AND, OR, XOR };

std::ostream &operator<<(std::ostream &os, GateOp op) {
    switch (op) {
    case GateOp::AND:
        os << "AND";
        break;
    case GateOp::OR:
        os << "OR";
        break;
    case GateOp::XOR:
        os << "XOR";
    }
    return os;
}

enum class CheckStatus { UNKNOWN = 0, GOOD, MAYBE_SWAPPED };

std::ostream &operator<<(std::ostream &os, CheckStatus status) {
    switch (status) {
    case CheckStatus::GOOD:
        os << "good";
        break;
    case CheckStatus::MAYBE_SWAPPED:
        os << "potentially swapped";
        break;
    case CheckStatus::UNKNOWN:
        os << "unknown";
    }
    return os;
}

struct Gate {
    std::string output;
    std::string input_1;
    std::string input_2;
    GateOp op;
    CheckStatus status = CheckStatus::UNKNOWN;
    bool swapped = false;
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

std::ostream &operator<<(std::ostream &os, const Gate &gate) {
    os << gate.output << " = " << gate.input_1 << " " << gate.op << " "
       << gate.input_2 << " (" << gate.status
       << (gate.swapped ? ", swapped" : "") << ")";
    return os;
}

class LogicSim {
    std::unordered_map<std::string, bool> values;
    std::vector<Gate> gates;
    /// mapping from a gate's output to the corresponding index for that gate
    std::unordered_map<std::string, std::size_t> gate_lookup;
    /// mapping from a gate's output to gates that use it as an input
    std::unordered_map<std::string, std::unordered_set<std::string>>
        connections;

    /// cached topological ordering of the gates
    std::vector<std::string> cached_eval_order{};

    void add_gate(Gate &&gate);
    bool eval_gate(const std::string &key) const;

    const std::vector<std::string> &get_eval_order();

    /**
     * Returns the formatted gate name for a specific variable and bit index.
     *
     * Examples:
     *  'z', 3 -> "z03"
     *  'y', 12 -> "y10"
     */
    std::string get_indexed_name(char variable, int index) const;

  public:
    int num_bits{};

    /// opaque handle representing a pair of swappable gates
    class SwapHandle {
        std::size_t first;
        std::size_t second;
        SwapHandle(std::size_t first, std::size_t second)
            : first(first), second(second) {}
        friend class LogicSim;
    };

    void evaluate(std::uint64_t x, std::uint64_t y);

    /// recursively marks the gates that are used by a specific bit in z,
    /// overwriting UNKNOWN and POTENTIALLY_SWAPPED values
    void mark(int bit, CheckStatus status);

    /// returns all pairs of potentially swapped gates, based on their marked
    /// status
    std::vector<SwapHandle> get_swaps() const;
    void swap_outputs(const SwapHandle &handle);
    void unswap_outputs(const SwapHandle &handle);
    void print_swap(std::ostream &, const SwapHandle &handle) const;

    std::string format_swapped_gates() const;

    std::uint64_t z_value() const;
    static std::tuple<LogicSim, std::uint64_t, std::uint64_t>
    read(std::istream &);
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

std::string LogicSim::get_indexed_name(char variable, int index) const {
    std::string key{variable};
    if (index < 10) {
        key += '0';
    }
    key += std::to_string(index);
    return key;
}

const std::vector<std::string> &LogicSim::get_eval_order() {
    if (!cached_eval_order.empty()) {
        return cached_eval_order;
    }

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
    cached_eval_order = aoc::graph::topo_sort(source, process_neighbors);
    if constexpr (aoc::DEBUG && false) {
        std::cerr << "eval_order: " << pretty_print::repr(cached_eval_order)
                  << "\n";
    }
    return cached_eval_order;
}

void LogicSim::evaluate(std::uint64_t x, std::uint64_t y) {
    values.clear();
    for (int i = 0; i < num_bits; ++i) {
        values[get_indexed_name('x', i)] = x & 1;
        values[get_indexed_name('y', i)] = y & 1;
        x >>= 1;
        y >>= 1;
    }

    const std::vector<std::string> &eval_order = get_eval_order();
    for (const auto &key : eval_order) {
        if (key.empty() || values.contains(key)) {
            continue;
        }
        values[key] = eval_gate(key);
    }
    if constexpr (aoc::DEBUG && false) {
        std::cerr << "values: " << pretty_print::repr(values) << "\n";
    }
}

std::uint64_t LogicSim::z_value() const {
    std::uint64_t z = 0;
    for (int i = 0; true; ++i) {
        if (auto it = values.find(get_indexed_name('z', i));
            it != values.end()) {
            if (it->second) {
                z |= 1ull << i;
            }
        } else {
            break;
        }
    }
    return z;
}

// part 2

void LogicSim::mark(int bit, CheckStatus status) {
    std::size_t output_index = gate_lookup.at(get_indexed_name('z', bit));
    if (gates[output_index].status == status) {
        return;
    }
    if constexpr (aoc::DEBUG) {
        std::cerr << "marking " << get_indexed_name('z', bit) << " as "
                  << status << "\n";
    }
    const auto process_neighbors = [this](std::size_t index, auto &&process) {
        const Gate &gate = gates[index];
        for (const auto &name : {gate.input_1, gate.input_2}) {
            // x* and y* inputs don't have corresponding gates, so we can't
            // just use gate_lookup.at()
            if (auto it = gate_lookup.find(name); it != gate_lookup.end()) {
                process(it->second);
            }
        }
    };
    const auto visit = [this, status](std::size_t index, int) -> bool {
        Gate &gate = gates[index];
        if (gate.status == CheckStatus::GOOD) {
            // stop recursing
            return false;
        }
        // mark the gate
        gate.status = status;
        if constexpr (aoc::DEBUG) {
            std::cerr << "  marked " << gate << "\n";
        }
        return true;
    };
    aoc::graph::bfs(output_index, process_neighbors, {}, visit);
}

std::vector<LogicSim::SwapHandle> LogicSim::get_swaps() const {
    std::vector<SwapHandle> swaps;
    // ordered list of candidate indices
    std::set<std::size_t> candidates;
    for (std::size_t i = 0; i < gates.size(); ++i) {
        if (gates[i].status == CheckStatus::MAYBE_SWAPPED) {
            candidates.emplace(i);
        }
    }

    std::unordered_map<std::size_t, std::set<std::size_t>> pred_cache;
    const auto get_predecessors =
        [this, &candidates,
         &pred_cache](const auto &rec,
                      std::size_t index) -> const std::set<std::size_t> & {
        if (auto it = pred_cache.find(index); it != pred_cache.end()) {
            // found entry in cache
            return it->second;
        }
        const Gate &gate = gates[index];
        std::set<std::size_t> &predecessors = pred_cache[index];
        for (const auto &name : {gate.input_1, gate.input_2}) {
            if (auto it = gate_lookup.find(name); it != gate_lookup.end()) {
                if (!candidates.contains(it->second)) {
                    // recursive base case
                    continue;
                }
                predecessors.insert(it->second);
                // recurse to get our input's predecessors, then merge them with
                // ours
                const auto &other_pred = rec(rec, it->second);
                for (std::size_t i : other_pred) {
                    predecessors.insert(i);
                }
            }
        }
        return predecessors;
    };

    if constexpr (aoc::DEBUG) {
        std::cerr << "swap candidates and their predecessors:\n";
    }

    // loop over ordered pairs of candidates
    for (std::size_t cand_1 : candidates) {
        const std::set<std::size_t> &preds_1 =
            get_predecessors(get_predecessors, cand_1);
        if constexpr (aoc::DEBUG) {
            std::cerr << "  " << gates[cand_1].output << ":";
            for (std::size_t pred : preds_1) {
                std::cerr << " " << gates[pred].output;
            }
            std::cerr << "\n";
        }
        for (std::size_t cand_2 : candidates) {
            if (cand_1 >= cand_2) {
                continue;
            }
            const std::set<std::size_t> &preds_2 =
                get_predecessors(get_predecessors, cand_2);
            if (preds_1.contains(cand_2) || preds_2.contains(cand_1)) {
                // the two candidates depend on each other, so swapping them
                // would create a loop
                continue;
            }
            swaps.push_back({cand_1, cand_2});
        }
    }

    return swaps;
}

void LogicSim::swap_outputs(const SwapHandle &handle) {
    Gate &gate_a = gates[handle.first];
    Gate &gate_b = gates[handle.second];
    // clear incoming connections
    connections[gate_a.input_1].erase(gate_a.output);
    connections[gate_a.input_2].erase(gate_a.output);
    connections[gate_b.input_1].erase(gate_b.output);
    connections[gate_b.input_2].erase(gate_b.output);
    // swap the outputs
    std::swap(gate_a.output, gate_b.output);
    std::swap(gate_lookup.at(gate_a.output), gate_lookup.at(gate_b.output));
    // rebuild connections
    connections[gate_a.input_1].insert(gate_a.output);
    connections[gate_a.input_2].insert(gate_a.output);
    connections[gate_b.input_1].insert(gate_b.output);
    connections[gate_b.input_2].insert(gate_b.output);
    gate_a.swapped = true;
    gate_b.swapped = true;
    cached_eval_order.clear();
}

void LogicSim::unswap_outputs(const SwapHandle &handle) {
    swap_outputs(handle);
    gates[handle.first].swapped = false;
    gates[handle.second].swapped = false;
}

void LogicSim::print_swap(std::ostream &os, const SwapHandle &handle) const {
    os << '{' << gates[handle.first].output << ", "
       << gates[handle.second].output << '}';
}

std::string LogicSim::format_swapped_gates() const {
    std::vector<std::string> names;
    for (const Gate &gate : gates) {
        if (gate.swapped) {
            names.push_back(gate.output);
        }
    }
    std::sort(names.begin(), names.end());

    std::ostringstream ss;
    bool first = true;
    for (const auto &name : names) {
        if (first) {
            first = false;
        } else {
            ss << ',';
        }
        ss << name;
    }
    return ss.str();
}

std::tuple<LogicSim, std::uint64_t, std::uint64_t>
LogicSim::read(std::istream &is) {
    std::string line;
    LogicSim sim;
    std::uint64_t x = 0, y = 0;
    int num_inputs;
    for (num_inputs = 0; std::getline(is, line); ++num_inputs) {
        if (line.empty()) {
            break;
        }
        if (line[5] == '1') {
            std::uint64_t value = 1ull << std::stoi(line.substr(1, 2));
            if (line[0] == 'x') {
                x |= value;
            } else if (line[0] == 'y') {
                y |= value;
            } else {
                assert(false);
            }
        }
    }
    sim.num_bits = num_inputs / 2;

    Gate gate{};
    while (is >> gate) {
        sim.add_gate(std::move(gate));
    }
    return {sim, x, y};
}

} // namespace aoc::day24

#endif /* end of include guard: DAY24_HPP_SZBNX4LR */
