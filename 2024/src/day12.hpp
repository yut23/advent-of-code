/******************************************************************************
 * File:        day12.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-12
 *****************************************************************************/

#ifndef DAY12_HPP_J8OYJLGO
#define DAY12_HPP_J8OYJLGO

#include "ds/grid.hpp"         // for Grid
#include "graph_traversal.hpp" // for bfs_manual_dedupe
#include "lib.hpp"             // for Pos, Delta, DIRECTIONS, DEBUG
#include "unit_test/pretty_print.hpp"
#include <functional> // for bind_front
#include <iomanip>    // for setw
#include <iostream>   // for istream
#include <utility>    // for move
#include <vector>     // for vector
// IWYU pragma: no_include <initializer_list>  // for DIRECTIONS

namespace aoc::day12 {
using aoc::ds::Grid;

struct Region {
    int index;
    char plant_type;
    int area = 0;
    int perimeter = 0;
    int corners = 0;

    template <aoc::Part part>
    int fence_price() const {
        if constexpr (part == PART_1) {
            return area * perimeter;
        } else {
            // number of sides == number of corners
            return area * corners;
        }
    }
};

class Garden {
    const Grid<char> plots;
    Grid<int> region_indices;
    std::vector<Region> regions;

    explicit Garden(Grid<char> &&plots)
        : plots(std::move(plots)), region_indices(plots, -1), regions() {}

    char get(const Pos &pos) const {
        if (plots.in_bounds(pos)) {
            return plots[pos];
        }
        return '.';
    }

    void process_plot(const Pos &pos, char plant_type);

  public:
    static Garden read(std::istream &is);
    template <aoc::Part part>
    int fence_price() const;
};

Garden Garden::read(std::istream &is) {
    Garden garden{aoc::ds::Grid<char>{aoc::read_lines(is)}};
    garden.plots.for_each_with_pos(
        std::bind_front(&Garden::process_plot, &garden));
    return garden;
}

void Garden::process_plot(const Pos &pos, char plant_type) {
    // flood-fill plots with matching plant types
    if (region_indices[pos] != -1) {
        // this plot is already part of a region
        return;
    }
    int region_index = regions.size();
    regions.push_back({region_index, plant_type});
    Region &region = regions.back();
    const auto process_neighbors = [this, &plant_type,
                                    &region](const Pos &p, auto &&process) {
        for (const auto &dir : DIRECTIONS) {
            Pos neighbor = p + Delta(dir);
            Delta shift_right =
                Delta(directions::turn(dir, RelDirection::right));
            if (get(neighbor) == plant_type) {
                // check for a concave corner
                if (get(p + shift_right) == plant_type &&
                    get(neighbor + shift_right) != plant_type) {
                    ++region.corners;
                }
                process(neighbor);
            } else {
                // neighboring plot is either outside the garden or growing
                // a different plant
                ++region.perimeter;
                // check for a convex corner
                if (get(p + shift_right) != plant_type) {
                    ++region.corners;
                }
            }
        }
    };
    const auto visit = [this, &region_index,
                        &region](const Pos &p, int /*distance*/) -> bool {
        if (region_indices[p] == region_index) {
            // already visited
            return false;
        }
        region_indices[p] = region_index;
        ++region.area;
        return true;
    };
    aoc::graph::bfs_manual_dedupe(pos, process_neighbors, /*is_target*/ {},
                                  visit);
}

template <aoc::Part part>
int Garden::fence_price() const {
    int total_price = 0;
    if constexpr (aoc::DEBUG) {
        std::cerr << "plots:\n" << pretty_print::repr(plots) << "\n";
        std::cerr << "regions:\n"
                  << std::setw(3) << pretty_print::repr(region_indices) << "\n";
    }
    for (const Region &region : regions) {
        if constexpr (aoc::DEBUG) {
            std::cerr << region.index << ": plant=" << region.plant_type
                      << ", price=" << region.fence_price<part>()
                      << "; area=" << region.area
                      << ", perimeter=" << region.perimeter
                      << ", corners=" << region.corners << "\n";
        }
        total_price += region.fence_price<part>();
    }
    return total_price;
}

} // namespace aoc::day12

#endif /* end of include guard: DAY12_HPP_J8OYJLGO */
