/******************************************************************************
 * File:        day20.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-21
 *****************************************************************************/

#ifndef DAY20_HPP_QRITMDTC
#define DAY20_HPP_QRITMDTC

#include <cassert>       // for assert
#include <iostream>      // for istream, ostream
#include <memory>        // for unique_ptr
#include <queue>         // for queue
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
    std::string source;
    std::string destination;
    MessageType type;
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

  protected:
    std::vector<std::string> connections;
    Message make_message(const std::string &dest, MessageType type) const {
        return Message{name, dest, type};
    }

  public:
    explicit ModuleBase(const std::string &name) : name(name), connections() {}
    virtual ~ModuleBase();

    void add_connection(std::string &&name) {
        connections.emplace_back(std::move(name));
    }
    virtual void setup_inputs(const std::vector<std::string> &) {}
    virtual void handle_message(MessageBus *, const Message &) {}
};

ModuleBase::~ModuleBase() {}

class FlipFlop : public ModuleBase {
    bool state = false;

  public:
    explicit FlipFlop(const std::string &name) : ModuleBase(name) {}
    void handle_message(MessageBus *bus, const Message &msg) override;
};

class Conjunction : public ModuleBase {
    std::unordered_map<std::string, MessageType> last_pulse;

  public:
    explicit Conjunction(const std::string &name) : ModuleBase(name) {}
    void setup_inputs(const std::vector<std::string> &names) override;
    void handle_message(MessageBus *bus, const Message &msg) override;
};

class Broadcaster : public ModuleBase {
  public:
    explicit Broadcaster(const std::string &name) : ModuleBase(name) {
        assert(name == "broadcaster");
    }
    void handle_message(MessageBus *bus, const Message &msg) override;
};

class MessageBus {
    std::unordered_map<std::string, std::unique_ptr<ModuleBase>> modules;
    std::queue<Message> msg_queue;
    int low_count = 0;
    int high_count = 0;

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

  public:
    static MessageBus read_modules(std::istream &is);

    void send_message(Message &&msg);
    bool process(bool debug = false);

    int get_low_count() const { return low_count; }
    int get_high_count() const { return high_count; }
};

void FlipFlop::handle_message(MessageBus *bus, const Message &msg) {
    if (msg.type == MessageType::low) {
        state = !state;
        // send pulse
        for (const std::string &conn : connections) {
            bus->send_message(make_message(conn, MessageType(state)));
        }
    }
}

void Conjunction::setup_inputs(const std::vector<std::string> &names) {
    for (const std::string &name : names) {
        last_pulse[name] = MessageType::low;
    }
}

void Conjunction::handle_message(MessageBus *bus, const Message &msg) {
    last_pulse[msg.source] = msg.type;
    MessageType type = MessageType::low;
    for (const auto &[_, last_type] : last_pulse) {
        if (last_type == MessageType::low) {
            type = MessageType::high;
            break;
        }
    }
    for (const std::string &conn : connections) {
        bus->send_message(make_message(conn, type));
    }
}

void Broadcaster::handle_message(MessageBus *bus, const Message &msg) {
    for (const std::string &conn : connections) {
        bus->send_message(make_message(conn, msg.type));
    }
}

void MessageBus::send_message(Message &&msg) {
    msg_queue.emplace(std::move(msg));
}

bool MessageBus::process(bool debug) {
    if (msg_queue.empty()) {
        return false;
    }
    const Message &msg = msg_queue.front();
    switch (msg.type) {
    case MessageType::low:
        ++low_count;
        break;
    case MessageType::high:
        ++high_count;
        break;
    default:
        assert(false);
    }
    if (debug) {
        std::cerr << msg << "\n";
    }
    auto it = modules.find(msg.destination);
    if (it != modules.end()) {
        it->second->handle_message(this, msg);
    }
    msg_queue.pop();
    return !msg_queue.empty();
}

MessageBus MessageBus::read_modules(std::istream &is) {
    MessageBus bus;
    std::string line;
    std::unordered_map<std::string, std::vector<std::string>> inputs;
    while (std::getline(is, line)) {
        std::istringstream ss{line};
        std::string tmp;
        ss >> tmp;
        ModuleBase *curr_module = nullptr;
        if (tmp == "broadcaster") {
            curr_module = bus.emplace_module<Broadcaster>(std::move(tmp));
        } else if (tmp[0] == '%') {
            tmp = tmp.substr(1);
            curr_module = bus.emplace_module<FlipFlop>(std::move(tmp));
        } else if (tmp[0] == '&') {
            tmp = tmp.substr(1);
            curr_module = bus.emplace_module<Conjunction>(std::move(tmp));
        } else {
            assert(false);
        }
        ss >> tmp;
        assert(tmp == "->");
        while (ss >> tmp) {
            if (tmp.back() == ',') {
                tmp.resize(tmp.size() - 1);
            }
            inputs[tmp].push_back(curr_module->name);
            curr_module->add_connection(std::move(tmp));
        }
    }

    // tell each module what it's inputs are
    for (const auto &[name, module] : bus.modules) {
        module->setup_inputs(inputs[name]);
    }

    return bus;
}

} // namespace aoc::day20

#endif /* end of include guard: DAY20_HPP_QRITMDTC */
