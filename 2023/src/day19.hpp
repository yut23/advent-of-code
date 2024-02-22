/******************************************************************************
 * File:        day19.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-19
 *****************************************************************************/

#ifndef DAY19_HPP_PFKAWO6L
#define DAY19_HPP_PFKAWO6L

#include "graph_traversal.hpp"
#include "lib.hpp"       // for expect_input, DEBUG
#include <algorithm>     // for find_if
#include <array>         // for array
#include <cctype>        // for isalpha
#include <compare>       // for strong_ordering
#include <cstddef>       // for size_t
#include <deque>         // for deque
#include <functional>    // for bind_front, multiplies
#include <iostream>      // for cerr, istream, ostream
#include <map>           // for multimap
#include <numeric>       // for transform_reduce
#include <optional>      // for optional
#include <sstream>       // for istringstream
#include <string>        // for string, getline
#include <unordered_map> // for unordered_map
#include <utility>       // for pair, move
#include <vector>        // for vector

namespace aoc::day19 {

using Part = std::array<int, 4>;

struct Condition {
    bool greater;
    unsigned char rating_idx;
    int threshold;

    Condition invert() const;
    bool matches(const Part &) const;

    bool operator==(const Condition &) const = default;
};

Condition Condition::invert() const {
    return {!greater, rating_idx, threshold + (greater ? +1 : -1)};
}

bool Condition::matches(const Part &part) const {
    if (greater) {
        return part[rating_idx] > threshold;
    } else {
        return part[rating_idx] < threshold;
    }
}

std::ostream &operator<<(std::ostream &os, const Condition &cond) {
    os << "xmas"[cond.rating_idx] << (cond.greater ? '>' : '<')
       << cond.threshold;
    return os;
}

struct Rule {
    std::optional<Condition> condition{};
    std::string dest;

    bool conditional() const { return condition.has_value(); }

    bool matches(const Part &) const;
};

bool Rule::matches(const Part &part) const {
    return !condition.has_value() || condition->matches(part);
}

struct Workflow {
    std::string name;
    std::vector<Rule> rules;

    const std::string &lookup(const Part &) const;

    static Workflow parse(const std::string &line);
};

const std::string &Workflow::lookup(const Part &part) const {
    return std::find_if(
               rules.begin(), rules.end(),
               [&part](const Rule &rule) { return rule.matches(part); })
        ->dest;
}

class PartCategorizer {
    std::unordered_map<std::string, Workflow> workflows{};

  public:
    void add_workflow(Workflow &&w) {
        workflows.try_emplace(w.name, std::move(w));
    }
    const Workflow &at(const std::string &name) const {
        return workflows.at(name);
    }
    const Rule &at(const std::string &name, std::size_t index) const {
        return workflows.at(name).rules[index];
    }
    bool process(const Part &) const;
};

///////////////////
// input parsing //
///////////////////

std::istream &read_part(std::istream &is, Part &part) {
    Part tmp;
    is >> aoc::expect_input("{x=") >> tmp[0] >> //
        aoc::expect_input(",m=") >> tmp[1] >>   // force clang-tidy to
        aoc::expect_input(",a=") >> tmp[2] >>   // break lines here
        aoc::expect_input(",s=") >> tmp[3] >>   //
        aoc::expect_input('}');
    if (is) {
        // only store to part if the entire stream was read correctly
        part = std::move(tmp);
    }
    return is;
}

std::istream &operator>>(std::istream &is, Condition &condition) {
    Condition cond;
    char ch;
    is >> ch;
    switch (ch) {
    case 'x':
        cond.rating_idx = 0;
        break;
    case 'm':
        cond.rating_idx = 1;
        break;
    case 'a':
        cond.rating_idx = 2;
        break;
    case 's':
        cond.rating_idx = 3;
        break;
    default:
        is.setstate(std::ios_base::failbit);
        return is;
    }
    is >> ch;
    if (ch != '>' && ch != '<') {
        is.setstate(std::ios_base::failbit);
        return is;
    }
    cond.greater = ch == '>';
    is >> cond.threshold;
    if (is) {
        condition = std::move(cond);
    }
    return is;
}

std::istream &operator>>(std::istream &is, Workflow &workflow) {
    Workflow w;
    std::getline(is, w.name, '{');
    char ch = '{';
    while (is && ch != '}') {
        Rule r{};
        char ch1, ch2;
        is >> ch1;
        ch2 = is.peek();
        if (ch2 == '>' || ch2 == '<') {
            // conditional rule
            is.unget();
            Condition cond{};
            is >> cond;
            if (is) {
                r.condition = std::move(cond);
            }
            is >> aoc::expect_input(':');
        } else {
            // unconditional rule
            r.condition = {};
            r.dest.push_back(ch1);
        }
        while (is >> ch && std::isalpha(ch)) {
            r.dest.push_back(ch);
        }
        if (is) {
            w.rules.push_back(std::move(r));
        }
    }
    if (is) {
        workflow = std::move(w);
    }
    return is;
}

std::pair<PartCategorizer, std::vector<Part>> read_input(std::istream &is) {
    std::string line;
    PartCategorizer cat{};
    while (std::getline(is, line)) {
        if (line.empty()) {
            // start reading parts
            break;
        }
        std::istringstream ss{line};
        Workflow w;
        if (ss >> w) {
            cat.add_workflow(std::move(w));
        }
    }
    std::vector<Part> parts;
    Part part;
    while (read_part(is, part)) {
        parts.push_back(std::move(part));
    }
    return {std::move(cat), std::move(parts)};
}

class Part2Solver {
  public:
    struct Key {
        const std::string name;
        const std::size_t index;

        Key(const std::string &name, std::size_t index)
            : name(name), index(index) {}
        Key(std::string &&name, std::size_t index)
            : name(std::move(name)), index(index) {}

        std::strong_ordering operator<=>(const Key &other) const = default;
    };

  private:
    const PartCategorizer &cat;

    const std::string in_key = "in";
    const Key source{in_key, 0};

    std::multimap<Key, Key> prev{};

    void process_neighbors(const Key &key, auto &&visit) const;

    void visit_with_parent(const Key &key, const Key &parent, int);

    std::optional<Condition> get_condition(const Key &from,
                                           const Key &to) const;

    static long count_combinations(const std::deque<Condition> &conditions);

  public:
    explicit Part2Solver(const PartCategorizer &cat) : cat(cat) {}

    long solve();
};

std::ostream &operator<<(std::ostream &os, const Part2Solver::Key &key) {
    os << "{" << key.name << ", " << key.index << "}";
    return os;
}

void Part2Solver::process_neighbors(const Key &key, auto &&visit) const {
    const auto &[name, index] = key;
    if (name == "A" || name == "R") {
        return;
    }
    const Rule &rule = cat.at(name, index);
    if (rule.conditional()) {
        visit({name, index + 1});
    }
    visit({rule.dest, 0});
}

void Part2Solver::visit_with_parent(const Key &key, const Key &parent, int) {
    prev.emplace(key, parent);
}

std::optional<Condition> Part2Solver::get_condition(const Key &from,
                                                    const Key &to) const {
    std::optional<Condition> cond = cat.at(from.name, from.index).condition;
    if (!cond) {
        return cond;
    }
    if (from.name == to.name) {
        // the condition should be false
        return cond->invert();
    } else {
        // the condition should be true
        return *cond;
    }
}

long Part2Solver::solve() {
    constexpr bool use_seen = false;
    aoc::graph::dfs<use_seen>(
        source,
        [this](const Key &key, auto &&visit) { process_neighbors(key, visit); },
        std::bind_front(&Part2Solver::visit_with_parent, this));

    if constexpr (aoc::DEBUG) {
        std::cerr << "digraph foobar {\n";
        for (const auto &[parent, key] : prev) {
            std::cerr << "  " << key.name << "_" << key.index << " -> "
                      << parent.name << "_" << parent.index << ";\n";
        }
        std::cerr << "}\n";
    }

    // get all paths to the A node
    std::deque<Condition> path{};
    const auto helper = [this, &path](const Key &key, const auto &rec) -> long {
        long combinations = 0;
        auto rng = prev.equal_range(key);
        for (auto it = rng.first; it != rng.second; ++it) {
            if (it->first == it->second) {
                // reached the start node
                combinations += count_combinations(path);
                continue;
            }
            std::optional<Condition> cond =
                get_condition(it->second, it->first);
            if (cond.has_value()) {
                // add the condition to the path
                path.push_front(cond.value());
            }
            // No two paths should ever produce overlapping ranges, as they
            // must differ in at least one conditional check.
            combinations += rec(it->second, rec);
            if (cond.has_value()) {
                // remove the condition from the path
                path.pop_front();
            }
        }
        return combinations;
    };

    std::string accepted = "A";
    Key accepted_key{accepted, 0};
    return helper(accepted_key, helper);
}

struct Range {
    int start = 1;
    int end = 4000;

    void add_condition(const Condition &cond);
    int size() const;
};

std::ostream &operator<<(std::ostream &os, const Range &r) {
    os << "Range(" << r.start << ", " << r.end << ")";
    return os;
}

void Range::add_condition(const Condition &cond) {
    if (cond.greater) {
        start = std::max(start, cond.threshold + 1);
    } else {
        end = std::min(end, cond.threshold - 1);
    }
}

int Range::size() const {
    if (start > end) {
        return 0;
    }
    return end - start + 1;
}

long Part2Solver::count_combinations(const std::deque<Condition> &conditions) {
    std::array<Range, 4> ranges;
    if constexpr (aoc::DEBUG) {
        std::cerr << "path:";
        for (const Condition &cond : conditions) {
            std::cerr << " " << cond;
        }
    }
    for (const Condition &cond : conditions) {
        ranges[cond.rating_idx].add_condition(cond);
    }
    long count = std::transform_reduce(ranges.begin(), ranges.end(), 1L,
                                       std::multiplies<>{},
                                       [](const Range &r) { return r.size(); });
    if constexpr (aoc::DEBUG) {
        std::cerr << ": " << count << " combinations\n";
    }
    return count;
}

} // namespace aoc::day19

#endif /* end of include guard: DAY19_HPP_PFKAWO6L */
