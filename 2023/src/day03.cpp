/******************************************************************************
 * File:        day03.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-03
 *****************************************************************************/

#include "lib.hpp"
#include <cctype>   // for isdigit
#include <iostream> // for cout, istream
#include <map>      // for map
#include <memory>   // for unique_ptr, make_unique
#include <string>   // for string, getline, stoi
#include <vector>   // for vector

namespace aoc::day03 {

struct Symbol {
    char value;
};

struct Label {
    Pos left;
    Pos right;
    std::string text{};
    Symbol *symbol{};

    explicit Label(const Pos &left) : left(left), right(left) {}
};

struct Grid {
    std::map<Pos, Symbol> symbols;
    std::vector<std::unique_ptr<Label>> labels;
    std::map<Pos, int> label_positions;

    int add_label(const Pos &pos, char c) {
        int index = labels.size();
        Label *label = labels.emplace_back(std::make_unique<Label>(pos)).get();
        label->text.push_back(c);
        label_positions[pos] = index;
        return index;
    }

    void extend_label(int index, char c) {
        Label *label;
        if constexpr (aoc::DEBUG) {
            label = labels.at(index).get();
        } else {
            label = labels[index].get();
        }
        label->text.push_back(c);
        label->right += Delta(Direction::right);
        label_positions[label->right] = index;
    }

    Label *get_label(const Pos &pos) {
        auto it = label_positions.find(pos);
        if (it != label_positions.end()) {
            return labels[it->second].get();
        }
        return nullptr;
    }

    void connect_labels() {
        for (auto &[pos, sym] : symbols) {
            // loop over all 8-way adjacent positions
            for (int i = -1; i <= 1; ++i) {
                for (int j = -1; j <= 1; ++j) {
                    Delta d(i, j);
                    Label *label = get_label(pos + d);
                    if (label == nullptr) {
                        continue;
                    }
                    label->symbol = &sym;
                }
            }
        }
    }
};

std::istream &operator>>(std::istream &is, Grid &grid) {
    std::string line;
    // read input line-by-line
    for (int y = 0; std::getline(is, line); ++y) {
        int label_idx = -1;
        for (int x = 0; x < static_cast<int>(line.size()); ++x) {
            if (std::isdigit(line[x])) {
                if (label_idx == -1) {
                    label_idx = grid.add_label(Pos(x, y), line[x]);
                } else {
                    grid.extend_label(label_idx, line[x]);
                }
            } else {
                label_idx = -1;
                if (line[x] != '.') {
                    grid.symbols[Pos(x, y)] = {line[x]};
                }
            }
        }
    }

    // associate labels with symbols
    grid.connect_labels();

    return is;
}

} // namespace aoc::day03

int main(int argc, char **argv) {
    std::ifstream infile = aoc::parse_args(argc, argv);

    aoc::day03::Grid grid;
    infile >> grid;

    int part_1 = 0;
    for (auto &label : grid.labels) {
        if (label->symbol != nullptr) {
            part_1 += std::stoi(label->text);
        }
    }
    std::cout << part_1 << "\n";

    return 0;
}
