/******************************************************************************
 * File:        day22.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2023-12-22
 *****************************************************************************/

#ifndef DAY22_HPP_9NHUADRF
#define DAY22_HPP_9NHUADRF

#include "lib.hpp"       // for expect_input, ranges_overlap, DEBUG
#include <algorithm>     // for sort, any_of, transform, move
#include <cassert>       // for assert
#include <compare>       // for strong_ordering
#include <deque>         // for deque
#include <iostream>      // for istream, ostream, cerr
#include <iterator>      // for back_inserter, inserter
#include <map>           // for map
#include <memory>        // for unique_ptr, make_unique
#include <set>           // for set
#include <unordered_map> // for unordered_map
#include <unordered_set> // for unordered_set
#include <utility>       // for move, pair
#include <vector>        // for vector

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
    struct BrickPtrCmp {
        template <class T, class U>
        bool operator()(const T &lhs, const U &rhs) const {
            return *lhs < *rhs;
        }
    };

    std::deque<BrickPtr> pending_bricks{};
    // bricks in their final positions, indexed by their upper z coordinate
    std::map<unsigned int, std::vector<BrickPtr>> settled_bricks{};
    // which bricks the key is resting on (i.e. those directly below)
    std::unordered_map<const Brick *, std::unordered_set<const Brick *>>
        supported_by{};
    // which bricks the key is supporting (i.e. those directly above)
    std::unordered_map<const Brick *, std::unordered_set<const Brick *>>
        supporting{};

  private:
    void mark_settled(BrickPtr &&brick,
                      std::unordered_set<const Brick *> &&overlaps);

    std::set<const Brick *, BrickPtrCmp> _bricks{};

  public:
    explicit BrickStack(std::vector<BrickPtr> &&bricks);

    bool settle_one();
    bool can_disintegrate(const Brick *candidate) const;
    bool is_unsafe(const Brick *brick) const;
    const std::set<const Brick *, BrickPtrCmp> &bricks() const {
        return _bricks;
    }

    static BrickStack read(std::istream &);
};

BrickStack::BrickStack(std::vector<BrickPtr> &&bricks) {
    // sort bricks according to their lower z coordinate
    std::sort(bricks.begin(), bricks.end(), BrickPtrCmp{});
    // store raw pointers to the bricks in _bricks, for users to iterate over
    std::transform(bricks.begin(), bricks.end(),
                   std::inserter(_bricks, _bricks.begin()),
                   [](const BrickPtr &bp) { return bp.get(); });
    // put everything into the pending queue
    std::move(bricks.begin(), bricks.end(), std::back_inserter(pending_bricks));
}

bool BrickStack::is_unsafe(const Brick *brick) const {
    for (const Brick *other : supporting.at(brick)) {
        if (supported_by.at(other).size() == 1) {
            return true;
        }
    }
    return false;
}

void BrickStack::mark_settled(BrickPtr &&brick,
                              std::unordered_set<const Brick *> &&overlaps) {
    unsigned int z = brick->p2.z;
    // ensure this brick has an entry in supporting
    supporting[brick.get()];
    for (const auto other : overlaps) {
        supporting[other].insert(brick.get());
    }
    supported_by[brick.get()] = std::move(overlaps);
    settled_bricks[z].push_back(std::move(brick));
}

bool BrickStack::settle_one() {
    if (pending_bricks.empty()) {
        return false;
    }
    // Bricks at the front of the (sorted) queue will never be obstructed by
    // any other brick in the queue. We only need to check for collisions with
    // settled bricks.
    BrickPtr curr = std::move(pending_bricks.front());
    pending_bricks.pop_front();

    unsigned int &z1 = curr->p1.z, &z2 = curr->p2.z;
    assert(z1 >= 1);
    assert(z2 >= z1);
    unsigned int height = z2 - z1 + 1;
    std::unordered_set<const Brick *> overlaps;
    for (; z1 > 1; --z1, --z2) {
        for (const BrickPtr &brick : settled_bricks[z1 - 1]) {
            if (curr->overlaps_xy(*brick)) {
                overlaps.insert(brick.get());
            }
        }
        if (!overlaps.empty()) {
            assert(z1 > 1);
            mark_settled(std::move(curr), std::move(overlaps));
            return !pending_bricks.empty();
        }
    }

    assert(height == z2 - z1 + 1);

    if (z1 == 1) {
        assert(overlaps.empty());
        mark_settled(std::move(curr), std::move(overlaps));
    }

    return !pending_bricks.empty();
}

bool BrickStack::can_disintegrate(const Brick *candidate) const {
    // find all bricks directly resting on the candidate, and all other bricks
    // that they could be resting on
    std::vector<const Brick *> resting_above;
    std::vector<const Brick *> neighbors;
    for (const Brick *brick : bricks()) {
        if (brick == candidate) {
            continue;
        }
        if (brick->p2.z == candidate->p2.z) {
            neighbors.push_back(brick);
        } else if (brick->p1.z == candidate->p2.z + 1 &&
                   candidate->overlaps_xy(*brick)) {
            resting_above.push_back(brick);
        }
    }

    if (resting_above.empty()) {
        if constexpr (aoc::DEBUG) {
            std::cerr << "brick " << *candidate << ": no bricks above\n";
        }
        return true;
    }

    // check that all bricks resting on this one are also supported by one of
    // the neighbors
    bool found_other_brick = true;
    for (const auto resting_brick : resting_above) {
        found_other_brick =
            std::any_of(neighbors.begin(), neighbors.end(),
                        [&resting_brick](const Brick *neighbor) {
                            return resting_brick->overlaps_xy(*neighbor);
                        });
        if (!found_other_brick) {
            if constexpr (aoc::DEBUG) {
                std::cerr << "brick " << *candidate
                          << ": no other brick supporting " << *resting_brick
                          << "\n";
            }
            break;
        }
    }
    return found_other_brick;
}

BrickStack BrickStack::read(std::istream &is) {
    std::vector<BrickPtr> all_bricks;
    while (is) {
        BrickPtr brick = std::make_unique<Brick>();
        if (is >> *brick) {
            all_bricks.push_back(std::move(brick));
        }
    }
    return BrickStack(std::move(all_bricks));
}

} // namespace aoc::day22

#endif /* end of include guard: DAY22_HPP_9NHUADRF */
