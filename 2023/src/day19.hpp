/******************************************************************************
 * File:        day19.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-19
 *****************************************************************************/

#ifndef DAY19_HPP_PFKAWO6L
#define DAY19_HPP_PFKAWO6L

#include "lib.hpp"       // for expect_input, DEBUG
#include <algorithm>     // for find_if
#include <array>         // for array
#include <cassert>       // for assert
#include <cctype>        // for isalpha
#include <functional>    // for hash (unordered_map)
#include <iostream>      // for istream
#include <sstream>       // for istringstream
#include <string>        // for string, getline
#include <unordered_map> // for unordered_map
#include <utility>       // for pair, move
#include <vector>        // for vector

namespace aoc::day19 {

using Part = std::array<int, 4>;

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

struct Rule {
    char condition;
    unsigned char rating_idx;
    int threshold;
    std::string dest;

    bool matches(const Part &) const;
};

bool Rule::matches(const Part &part) const {
    if (condition == '<') {
        return part[rating_idx] < threshold;
    } else if (condition == '>') {
        return part[rating_idx] > threshold;
    } else {
        return true;
    }
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
            is >> r.condition;
            switch (ch1) {
            case 'x':
                r.rating_idx = 0;
                break;
            case 'm':
                r.rating_idx = 1;
                break;
            case 'a':
                r.rating_idx = 2;
                break;
            case 's':
                r.rating_idx = 3;
                break;
            default:
                assert(false);
            }
            is >> r.threshold;
            is >> aoc::expect_input(':');
        } else {
            // unconditional rule
            r.condition = '*';
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

class PartCategorizer {
    std::unordered_map<std::string, Workflow> workflows{};

  public:
    void add_workflow(Workflow &&w) {
        workflows.try_emplace(w.name, std::move(w));
    }
    bool process(const Part &) const;
};

bool PartCategorizer::process(const Part &part) const {
    std::string curr = "in";
    if constexpr (aoc::DEBUG) {
        std::cerr << "processing part {x=" << part[0] << ",m=" << part[1]
                  << ",a=" << part[2] << ",s=" << part[3] << "}: in";
    }
    while (curr != "A" && curr != "R") {
        curr = workflows.at(curr).lookup(part);
        if constexpr (aoc::DEBUG) {
            std::cerr << " -> " << curr;
        }
    }
    if constexpr (aoc::DEBUG) {
        std::cerr << "\n";
    }
    return curr == "A";
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
    return {cat, parts};
}

} // namespace aoc::day19

#endif /* end of include guard: DAY19_HPP_PFKAWO6L */
