/******************************************************************************
 * File:        day09.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-12-09
 *****************************************************************************/

#ifndef DAY09_HPP_MBAPIACN
#define DAY09_HPP_MBAPIACN

#include "ds/grid.hpp"         // for Grid
#include "graph_traversal.hpp" // for bfs
#include "lib.hpp" // for Pos, Delta, relative_to, expect_input, read_vector, AbsDirection, RelDirection, DEBUG

#include <cassert>  // for assert
#include <cstdlib>  // for abs, size_t
#include <iostream> // for ostream, cerr, istream, boolalpha
#include <iterator> // for distance
#include <set>      // for set
#include <utility>  // for move
#include <vector>   // for vector

namespace aoc {
std::istream &operator>>(std::istream &is, Pos &pos) {
    Pos tmp;
    is >> tmp.x >> aoc::expect_input(',') >> tmp.y;
    if (is) {
        pos = std::move(tmp);
    }
    return is;
}
} // namespace aoc

namespace aoc::day09 {

auto read_input(std::istream &is) { return aoc::read_vector<Pos>(is); }

long calc_area(const Pos &p1, const Pos p2) {
    Delta diff = p2 - p1;
    return static_cast<long>(std::abs(diff.dx) + 1) * (std::abs(diff.dy) + 1);
}

struct Tile {
    bool inside : 1 = false;
    bool vert_edge : 1 = false;
    bool horz_edge : 1 = false;
    bool corner : 1 = false;

    void set_corner() {
        inside = true;
        corner = true;
    }
};

std::ostream &operator<<(std::ostream &os, Tile tile) {
    if (tile.corner) {
        os << '#';
    } else if (tile.vert_edge && tile.horz_edge) {
        os << '+';
    } else if (tile.vert_edge) {
        os << '|';
    } else if (tile.horz_edge) {
        os << '-';
    } else if (tile.inside) {
        os << 'O';
    } else {
        os << '.';
    }
    return os;
}

constexpr int sgn(int x) {
    if (x == 0) {
        return 0;
    }
    return x < 0 ? -1 : 1;
}

long solve_part_2(const std::vector<Pos> &points, const std::set<int> &x_set,
                  const std::set<int> &y_set) {
    std::vector<int> x_values(x_set.begin(), x_set.end()),
        y_values(y_set.begin(), y_set.end());

    std::vector<Pos> compressed_points;
    for (std::size_t i = 0; i < points.size(); ++i) {
        compressed_points.emplace_back(
            std::distance(x_set.begin(), x_set.find(points[i].x)),
            std::distance(y_set.begin(), y_set.find(points[i].y)));
    }
    assert(!compressed_points.empty());
    aoc::ds::Grid<Tile> grid(static_cast<int>(x_set.size()),
                             static_cast<int>(y_set.size()));
    int right_turn_count = 0;
    for (std::size_t i = 0; i < compressed_points.size(); ++i) {
        Pos prev = compressed_points[(i + compressed_points.size() - 1) %
                                     compressed_points.size()];
        Pos curr = compressed_points[i];
        Pos next = compressed_points[(i + 1) % compressed_points.size()];
        AbsDirection prev_dir = (curr - prev).to_unit_vector().to_direction();
        Delta inc = (next - curr).to_unit_vector();
        AbsDirection next_dir = inc.to_direction();
        if (directions::relative_to(next_dir, prev_dir) ==
            RelDirection::right) {
            ++right_turn_count;
        } else {
            --right_turn_count;
        }
        grid[curr].corner = true;
        grid[curr].inside = true;
        assert(inc.dx != 0 || inc.dy != 0);
        while ((curr += inc) != next) {
            Tile &tile = grid[curr];
            tile.inside = true;
            if (inc.dx == 0) {
                tile.vert_edge = true;
            } else {
                tile.horz_edge = true;
            }
        }
    }

    assert(right_turn_count == 4 || right_turn_count == -4);
    {
        Pos prev_corner = compressed_points[0];
        Pos curr = compressed_points[1];
        Pos next_corner = compressed_points[2];
        AbsDirection prev_dir =
            (curr - prev_corner).to_unit_vector().to_direction();
        AbsDirection next_dir =
            (next_corner - curr).to_unit_vector().to_direction();
        Delta shift = (curr - prev_corner).to_unit_vector() +
                      (curr - next_corner).to_unit_vector();
        bool is_right_turn =
            directions::relative_to(next_dir, prev_dir) == RelDirection::right;
        Pos inside;
        if ((right_turn_count > 0) ^ is_right_turn) {
            inside = curr + shift;
        } else {
            inside = curr - shift;
        }

        // flood fill the inside
        const auto process_neighbors = [&grid](const Pos &pos,
                                               const auto &process) {
            grid.manhattan_kernel(pos, [&](const Tile &tile, const Pos &p) {
                if (!tile.inside) {
                    process(p);
                }
            });
        };
        const auto visit = [&](const Pos &p, int /*distance*/) {
            grid[p].inside = true;
        };
        // cppcheck-suppress unreadVariable
        constexpr bool use_seen = false;
        aoc::graph::bfs<use_seen>(inside, process_neighbors, {}, visit);

        if constexpr (aoc::DEBUG) {
            std::cerr << "right turns: " << right_turn_count << "\n";
            std::cerr << "is right turn: " << std::boolalpha << is_right_turn
                      << "\n";
            std::cerr << "inside:  " << inside << "\n";
            const auto formatter = [&](Tile tile, Pos pos) {
                if (pos == inside) {
                    std::cerr << "\033[31m";
                }
                if (pos == compressed_points[0]) {
                    std::cerr << 'X';
                } else {
                    std::cerr << tile;
                }
                if (pos == inside) {
                    std::cerr << "\033[0m";
                }
            };
            grid.custom_print(std::cerr, formatter);
        }
    }

    const auto check_edges = [&](const Pos &a, const Pos &b) {
        Pos curr = a;
        Delta a_to_b = b - a, step{0, 0};
        int count = 0;
        for (int i = 0; i < 4; ++i) {
            switch (i) {
            case 0:
                // step along x towards b
                step = {sgn(a_to_b.dx), 0};
                count = std::abs(a_to_b.dx);
                break;
            case 1:
                // step along y towards b
                step = {0, sgn(a_to_b.dy)};
                count = std::abs(a_to_b.dy);
                break;
            case 2:
                // step along x towards a
                step = {-sgn(a_to_b.dx), 0};
                count = std::abs(a_to_b.dx);
                break;
            case 3:
                // step along y towards a
                step = {0, -sgn(a_to_b.dy)};
                count = std::abs(a_to_b.dy);
                break;
            }
            for (int j = 0; j < count; ++j, curr += step) {
                if (!grid[curr].inside) {
                    return false;
                }
            }
        }
        return true;
    };

    // loop over all starting points
    long max_area = 0;
    for (std::size_t i = 0; i < compressed_points.size(); ++i) {
        const auto &cp_i = compressed_points[i];
        const auto &p_i = points[i];
        for (std::size_t j = i + 1; j < compressed_points.size(); ++j) {
            long area = aoc::day09::calc_area(p_i, points[j]);
            if (area <= max_area) {
                continue;
            }
            // check validity along all edges
            if (check_edges(cp_i, compressed_points[j])) {
                max_area = area;
            }
        }
    }

    return max_area;
}

} // namespace aoc::day09

#endif /* end of include guard: DAY09_HPP_MBAPIACN */
