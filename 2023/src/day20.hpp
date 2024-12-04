/******************************************************************************
 * File:        day20.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-21
 *****************************************************************************/

#ifndef DAY20_HPP_QRITMDTC
#define DAY20_HPP_QRITMDTC

#include "graph_traversal.hpp"
#include "lib.hpp"       // for DEBUG
#include <algorithm>     // for all_of, transform, lower_bound, sort
#include <cassert>       // for assert
#include <cstddef>       // for size_t
#include <iostream>      // for istream, ostream, cerr, cout
#include <iterator>      // for back_inserter, distance
#include <memory>        // for unique_ptr, make_unique
#include <numeric>       // for lcm
#include <queue>         // for queue
#include <set>           // for set (tarjan_scc)
#include <sstream>       // for istringstream
#include <string>        // for string, getline
#include <unordered_map> // for unordered_map
#include <utility>       // for move, forward
#include <vector>        // for vector
// IWYU pragma: no_include <functional>  // for hash (unordered_map)

namespace aoc::day20 {

enum class MessageType : bool {
    low = false,
    high = true,
};

struct Message {
    const std::string &source;
    const std::string &destination;
    MessageType type;
};

enum class ModuleType {
    flipflop,
    conjunction,
    broadcaster,
    dummy,
};

std::ostream &operator<<(std::ostream &os, const Message &msg) {
    os << msg.source << " -" << (msg.type == MessageType::low ? "low" : "high")
       << "-> " << msg.destination;
    return os;
}

class MessageBus;

class ModuleBase {
  public:
    const std::string name;
    const ModuleType type;
    int component_id = -1;

  protected:
    std::vector<std::string> inputs;
    std::vector<std::string> outputs;
    void send_message(MessageBus *bus, MessageType type) const;
    virtual void dump_state(std::vector<bool> &) const {}

  public:
    ModuleBase(const std::string &name, ModuleType type)
        : name(name), type(type), inputs(), outputs() {}
    virtual ~ModuleBase();

    void add_output(std::string &&name) {
        outputs.emplace_back(std::move(name));
    }
    virtual std::string to_string() const { return name; }
    virtual void setup_inputs(const std::vector<std::string> &names) {
        inputs = names;
    }
    virtual void handle_message(MessageBus *, const Message &) {}

    friend MessageBus;
};

ModuleBase::~ModuleBase() {}

struct MessageCounter {
    int low = 0;
    int high = 0;

    int total() const { return low + high; }

    void count(const Message &msg) {
        switch (msg.type) {
        case MessageType::low:
            ++low;
            break;
        case MessageType::high:
            ++high;
            break;
        default:
            assert(false);
        }
    }
};

struct CycleInfo {
    std::size_t component_id;
    long length = -1;
    std::vector<long> low_presses{};
    long delay = 0;
    MessageCounter counter{};
};

class MessageBus {
    std::unordered_map<std::string, std::unique_ptr<ModuleBase>> modules;
    std::queue<Message> msg_queue;
    std::vector<std::vector<std::string>> components{};
    MessageCounter counter;

    bool _rx_activated = false;

    template <class T, class... Args>
        requires std::is_base_of_v<ModuleBase, T>
    ModuleBase *emplace_module(Args &&...args) {
        std::unique_ptr<ModuleBase> module =
            std::make_unique<T>(std::forward<Args>(args)...);
        auto [it, inserted] =
            modules.try_emplace(module->name, std::move(module));
        assert(inserted);
        return it->second.get();
    }

    ModuleBase *get_module(const std::string &name) const {
        if (auto it = modules.find(name); it != modules.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    void identify_components();

    // cycle detection
    std::unordered_map<std::size_t, CycleInfo> cycles{};
    std::vector<std::vector<bool>> initial_states{};
    std::vector<std::vector<bool>> dump_component_states() const;

  public:
    static MessageBus read_modules(std::istream &is);

    void send_message(Message &&msg);
    bool process(long presses, bool debug = false);

    int low_count() const { return counter.low; }
    int high_count() const { return counter.high; }
    bool has_rx() const { return modules.contains("rx"); }
    bool rx_activated() const { return _rx_activated; }

    bool check_cycles(long presses);
    long calculate_finish() const;
};

void ModuleBase::send_message(MessageBus *bus, MessageType type) const {
    for (const std::string &conn : outputs) {
        bus->send_message(Message{name, conn, type});
    }
}

void MessageBus::send_message(Message &&msg) {
    msg_queue.emplace(std::move(msg));
}

bool MessageBus::process(long presses, bool debug) {
    if (msg_queue.empty()) {
        return false;
    }
    const Message &msg = msg_queue.front();
    counter.count(msg);
    if (msg.destination == "rx" && msg.type == MessageType::low) {
        _rx_activated = true;
    } else if (debug) {
        std::cerr << msg << "\n";
    }
    const ModuleBase *src = get_module(msg.source);
    ModuleBase *dest = get_module(msg.destination);
    if (src != nullptr && src->component_id != dest->component_id &&
        cycles.contains(src->component_id)) {
        CycleInfo &info = cycles.at(src->component_id);
        info.counter.count(msg);
        if (msg.type == MessageType::low) {
            info.low_presses.push_back(presses);
        }
    }
    assert(dest != nullptr);
    dest->handle_message(this, msg);
    msg_queue.pop();
    return !msg_queue.empty();
}

namespace modules {
class FlipFlop : public ModuleBase {
    bool state = false;

  protected:
    void dump_state(std::vector<bool> &vec) const override {
        vec.push_back(state);
    }

  public:
    explicit FlipFlop(const std::string &name)
        : ModuleBase(name, ModuleType::flipflop) {}
    std::string to_string() const override { return "%" + name; }
    void handle_message(MessageBus *bus, const Message &msg) override {
        if (msg.type == MessageType::low) {
            state = !state;
            // send pulse
            send_message(bus, state ? MessageType::high : MessageType::low);
        }
    }
};

class Conjunction : public ModuleBase {
    std::vector<MessageType> last_pulse;

  protected:
    void dump_state(std::vector<bool> &vec) const override {
        std::transform(last_pulse.begin(), last_pulse.end(),
                       std::back_inserter(vec),
                       [](MessageType mt) { return mt == MessageType::high; });
    }

  public:
    explicit Conjunction(const std::string &name)
        : ModuleBase(name, ModuleType::conjunction) {}
    std::string to_string() const override { return "&" + name; }
    void setup_inputs(const std::vector<std::string> &names) override {
        inputs = names;
        std::sort(inputs.begin(), inputs.end());
        last_pulse.resize(inputs.size(), MessageType::low);
    }
    void handle_message(MessageBus *bus, const Message &msg) override {
        std::size_t index = std::distance(
            inputs.begin(),
            std::lower_bound(inputs.begin(), inputs.end(), msg.source));
        assert(index < inputs.size());
        last_pulse[index] = msg.type;
        bool send_low =
            std::all_of(last_pulse.begin(), last_pulse.end(),
                        [](MessageType mt) { return mt == MessageType::high; });
        send_message(bus, send_low ? MessageType::low : MessageType::high);
    }
};

class Broadcaster : public ModuleBase {
  public:
    explicit Broadcaster(const std::string &name)
        : ModuleBase(name, ModuleType::broadcaster) {
        assert(name == "broadcaster");
    }
    void handle_message(MessageBus *bus, const Message &msg) override {
        send_message(bus, msg.type);
    }
};
} // namespace modules

MessageBus MessageBus::read_modules(std::istream &is) {
    MessageBus bus;
    std::string line;
    std::unordered_map<std::string, std::vector<std::string>> inputs;
    constexpr bool write_graph = false;
    if constexpr (write_graph) {
        std::cout << "digraph G {\n";
    }
    while (std::getline(is, line)) {
        std::istringstream ss{line};
        std::string tmp;
        ss >> tmp;
        ModuleBase *curr_module = nullptr;
        if (tmp == "broadcaster") {
            curr_module =
                bus.emplace_module<modules::Broadcaster>(std::move(tmp));
        } else if (tmp[0] == '%') {
            tmp = tmp.substr(1);
            curr_module = bus.emplace_module<modules::FlipFlop>(std::move(tmp));
        } else if (tmp[0] == '&') {
            tmp = tmp.substr(1);
            curr_module =
                bus.emplace_module<modules::Conjunction>(std::move(tmp));
        } else {
            assert(false);
        }
        if constexpr (write_graph) {
            std::cout << "  " << curr_module->name << " [label=\""
                      << curr_module->to_string() << "\"];\n";
        }
        ss >> tmp;
        assert(tmp == "->");
        while (ss >> tmp) {
            if (tmp.back() == ',') {
                tmp.resize(tmp.size() - 1);
            }
            inputs[tmp].push_back(curr_module->name);
            if constexpr (write_graph) {
                std::cout << "  " << curr_module->name << " -> " << tmp
                          << ";\n";
            }
            curr_module->add_output(std::move(tmp));
        }
    }
    if constexpr (write_graph) {
        std::cout << "}\n";
    }

    // tell each module what it's inputs are, and identify dummy modules
    for (const auto &[name, module] : bus.modules) {
        module->setup_inputs(inputs[name]);
        inputs.erase(name);
    }
    for (const auto &[name, predecessors] : inputs) {
        if constexpr (aoc::DEBUG) {
            std::cerr << "dummy module: " << name << "\n";
        }
        ModuleBase *module =
            bus.emplace_module<ModuleBase>(name, ModuleType::dummy);
        module->setup_inputs(predecessors);
    }

    bus.identify_components();

    bus.initial_states = bus.dump_component_states();

    return bus;
}

void MessageBus::identify_components() {
    const std::string root = "broadcaster";
    const auto process_neighbors = [this](const std::string &name,
                                          auto &&visit) {
        std::ranges::for_each(modules.at(name)->outputs, visit);
    };
    components = aoc::graph::tarjan_scc(root, process_neighbors).first;

    // store component ids in each module
    for (std::size_t i = 0; i < components.size(); ++i) {
        for (const std::string &name : components[i]) {
            modules.at(name)->component_id = i;
        }
    }

    for (std::size_t i = 0; i < components.size(); ++i) {
        if (components[i].size() > 1) {
            cycles.try_emplace(i, CycleInfo{i});
        }
    }

    if constexpr (aoc::DEBUG) {
        std::cerr << "components:\n";
        for (std::size_t i = 0; i < components.size(); ++i) {
            const auto &component = components[i];
            std::cerr << "  " << i << ": {";
            for (auto it = component.begin(); it != component.end(); ++it) {
                if (it != component.begin()) {
                    std::cerr << ", ";
                }
                std::cerr << modules.at(*it)->to_string();
            }
            std::cerr << "}\n";
        }
    }

    if (has_rx()) {
        assert(modules.at("rx")->component_id ==
               static_cast<int>(components.size() - 1));
    }
    int broadcaster_id = modules.at("broadcaster")->component_id;
    assert(broadcaster_id == 0);
    assert(components[broadcaster_id].size() == 1);
}

std::vector<std::vector<bool>> MessageBus::dump_component_states() const {
    std::vector<std::vector<bool>> component_states(components.size());
    if (!initial_states.empty()) {
        assert(initial_states.size() == components.size());
        for (std::size_t i = 0; i < initial_states.size(); ++i) {
            component_states[i].reserve(initial_states[i].size());
        }
    }
    for (const auto &[name, module] : modules) {
        module->dump_state(component_states[module->component_id]);
    }
    return component_states;
}

bool MessageBus::check_cycles(long presses) {
    std::vector<std::vector<bool>> current_states = dump_component_states();
    bool done = true;
    for (auto &[i, info] : cycles) {
        if (info.length != -1) {
            continue;
        }
        if (current_states[i] == initial_states[i]) {
            info.length = presses;
            if constexpr (aoc::DEBUG) {
                std::cerr << "found cycle in component " << i << " after "
                          << presses << " presses; sent "
                          << info.counter.total()
                          << " messages since start of cycle: "
                          << info.counter.low << " low, " << info.counter.high
                          << " high\n";
            }
        } else {
            done = false;
        }
    }
    return done;
}

long MessageBus::calculate_finish() const {
    long lcm = 1;
    for (auto &[_, info] : cycles) {
        assert(info.low_presses.size() == 1);
        assert(info.low_presses[0] == info.length);
        lcm = std::lcm(lcm, info.length);
    }
    return lcm;
}

} // namespace aoc::day20

// vim: sw=4
#endif /* end of include guard: DAY20_HPP_QRITMDTC */
