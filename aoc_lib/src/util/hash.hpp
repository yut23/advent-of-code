/******************************************************************************
 * File:        hash.hpp
 *
 * Author:      Eric T. Johnson
 * Created:     2024-03-18
 * Description: Hashing utilities
 *****************************************************************************/

#ifndef HASH_HPP_QKWFTL3O
#define HASH_HPP_QKWFTL3O

#include "util/concepts.hpp" // for Hashable
#include <algorithm>         // for max  // IWYU pragma: keep
#include <cstdint>           // for uint64_t
#include <cstdlib>           // for size_t
#include <functional>        // for hash
// IWYU pragma: no_include <unordered_map>  // for operator==

namespace util {

namespace hash {
/**
 * Combiner from MurmurHash2, 64-bit version by Austin Appleby
 * (MurmurHash64A()). Placed in the public domain.
 */
void murmurhash2_combine(std::size_t &h, std::size_t k) {
    const std::uint64_t m = 0xc6a4a7935bd1e995ull;
    const int r = 47;

    k *= m;
    k ^= k >> r;
    k *= m;

    h ^= k;
    h *= m;
}

void murmurhash2_finalize(std::size_t &h) {
    const std::uint64_t m = 0xc6a4a7935bd1e995ull;
    const int r = 47;

    h ^= h >> r;
    h *= m;
    h ^= h >> r;
}

/**
 * Generate a seed with `hexdump -n8 -e '"0x" 8/1 "%02x" "ull\n"' </dev/urandom`
 */
template <util::concepts::Hashable... Ts>
void make_hash(std::size_t &seed, const Ts... components) {
    (hash::murmurhash2_combine(seed, std::hash<Ts>{}(components)), ...);
    hash::murmurhash2_finalize(seed);
}

/**
 * From https://artificial-mind.net/blog/2021/10/09/unordered-map-badness.
 *
 * > A badness of roughly 0 means that the current bucket distribution is close
 * > to optimal. 1 means that on average 100% more comparisons than optimal are
 * > required.
 */
template <class Map>
double unordered_map_badness(Map const &map) {
    auto const lambda = map.size() / double(map.bucket_count());

    auto cost = 0.;
    for (auto const &[k, _] : map)
        cost += map.bucket_size(map.bucket(k));
    cost /= map.size();

    return std::max(0., cost / (1 + lambda) - 1);
}
} // namespace hash

using hash::make_hash;

} // namespace util

#endif /* end of include guard: HASH_HPP_QKWFTL3O */
