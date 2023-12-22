/******************************************************************************
 * File:        day22.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-22
 *****************************************************************************/

#ifndef DAY22_HPP_9NHUADRF
#define DAY22_HPP_9NHUADRF

#include "lib.hpp"       // for expect_input, ranges_overlap
#include <algorithm>     // for copy, sort
#include <cassert>       // for assert
#include <compare>       // for strong_ordering
#include <deque>         // for deque
#include <iostream>      // for istream, ostream
#include <iterator>      // for istream_iterator, back_inserter
#include <map>           // for multimap
#include <memory>        // for unique_ptr, make_unique
#include <unordered_set> // for unordered_set
#include <utility>       // for move
#include <vector>        // for vector

#include "unit_test/util.hpp"

namespace aoc::day22 {

struct Pos3 {
    unsigned int x;
    unsigned int y;
    unsigned int z;

    std::strong_ordering operator<=>(const Pos3 &other) const {
        if (auto cmp = z <=> other.z; cmp != 0)
            return cmp;
        if (auto cmp = x <=> other.x; cmp != 0)
            return cmp;
        return y <=> other.y;
    }
};

std::istream &operator>>(std::istream &is, Pos3 &pos) {
    Pos3 p;
    if (is >> p.x >> aoc::expect_input(',') >> p.y >> aoc::expect_input(',') >>
        p.z) {
        pos = std::move(p);
    }
    return is;
}

std::ostream &operator<<(std::ostream &os, const Pos3 pos) {
    os << pos.x << ',' << pos.y << ',' << pos.z;
    return os;
}

struct Brick {
    Pos3 p1;
    Pos3 p2;

    bool overlaps_xy(const Brick &other) const;

    std::strong_ordering operator<=>(const Brick &) const = default;
};

bool Brick::overlaps_xy(const Brick &other) const {
    return ranges_overlap<std::pair<int, int>>({p1.x, p2.x},
                                               {other.p1.x, other.p2.x}) &&
           ranges_overlap<std::pair<int, int>>({p1.y, p2.y},
                                               {other.p1.y, other.p2.y});
}

std::istream &operator>>(std::istream &is, Brick &brick) {
    Brick b;
    is >> b.p1 >> aoc::expect_input('~') >> b.p2;
    if (is) {
        brick = std::move(b);
        assert(brick.p1.x <= brick.p2.x);
        assert(brick.p1.y <= brick.p2.y);
        assert(brick.p1.z <= brick.p2.z);
    }
    return is;
}

std::ostream &operator<<(std::ostream &os, const Brick &brick) {
    os << brick.p1 << '~' << brick.p2;
    return os;
}

struct BrickStack {
    using BrickPtr = std::unique_ptr<Brick>;
    std::deque<BrickPtr> pending_bricks{};
    // bricks in their final positions, indexed by their upper z coordinate
    std::multimap<unsigned int, BrickPtr> settled_bricks{};
    // bricks that are not safe to disintegrate
    std::unordered_set<const Brick *> unsafe{};

  private:
    void mark_settled(BrickPtr &&brick,
                      const std::vector<const Brick *> &overlaps);

  public:
    explicit BrickStack(std::vector<BrickPtr> &&bricks);

    bool settle_one();

    static BrickStack read(std::istream &);
};

BrickStack::BrickStack(std::vector<BrickPtr> &&bricks) {
    // sort bricks according to their lower z coordinate
    std::sort(bricks.begin(), bricks.end());
    auto ground_end =
        std::find_if(bricks.begin(), bricks.end(),
                     [](const BrickPtr &b) { return b->p1.z > 0; });
    // mark any bricks that start out touching the ground as settled
    for (auto it = bricks.begin(); it != ground_end; ++it) {
        mark_settled(std::move(*it), {});
    }
    // put everything else into the pending queue
    std::move(ground_end, bricks.end(), std::back_inserter(pending_bricks));
}

void BrickStack::mark_settled(BrickPtr &&brick,
                              const std::vector<const Brick *> &overlaps) {
    settled_bricks.emplace(brick->p2.z, std::move(brick));
    if (overlaps.size() == 1) {
        unsafe.emplace(overlaps[0]);
    }
}

bool BrickStack::settle_one() {
    // Bricks at the front of the (sorted) queue will never be obstructed by any
    // other block in the queue. We only need to check
    BrickPtr curr = std::move(pending_bricks.front());
    pending_bricks.pop_front();

    unsigned int &z1 = curr->p1.z, &z2 = curr->p2.z;
    for (; z1 > 0; --z1, --z2) {
        auto range = settled_bricks.equal_range(z1 - 1);
        std::vector<const Brick *> overlaps;
        for (auto it = range.first; it != range.second; ++it) {
            if (curr->overlaps_xy(*it->second)) {
                overlaps.push_back(it->second.get());
            }
        }
        if (!overlaps.empty()) {
            mark_settled(std::move(curr), overlaps);
            break;
        }
    }

    if (z1 == 0) {
        mark_settled(std::move(curr), {});
    }

    return !pending_bricks.empty();
}

BrickStack BrickStack::read(std::istream &is) {
    std::vector<BrickPtr> bricks;
    while (is) {
        BrickPtr brick = std::make_unique<Brick>();
        if (is >> *brick) {
            bricks.push_back(std::move(brick));
        }
    }
    return BrickStack(std::move(bricks));
}

} // namespace aoc::day22

#endif /* end of include guard: DAY22_HPP_9NHUADRF */
