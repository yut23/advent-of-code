/******************************************************************************
 * File:        concepts.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-01-16
 * Description: Concepts and type traits.
 *****************************************************************************/

#ifndef CONCEPTS_HPP_0EY957BS
#define CONCEPTS_HPP_0EY957BS

#include <concepts>   // for convertible_to, same_as
#include <cstddef>    // for size_t
#include <functional> // for hash
#include <iostream>   // for ostream
#include <iterator>   // for forward_iterator
#include <ranges>     // for range, range_value_t
#include <utility>    // for pair

namespace util::concepts {

// Declaration of the concept "Hashable", which is satisfied by any type 'T'
// such that for values 'a' of type 'T', the expression std::hash<T>{}(a)
// compiles and its result is convertible to std::size_t
template <typename T>
// need to update to clang-format 16+
// clang-format off
concept Hashable =
    requires(T a) {
        { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
    };
// clang-format on

template <typename C, typename T>
// clang-format off
concept any_iterable_collection =
    std::same_as<typename C::value_type, T> &&
    requires (C c) {
        { std::begin(c) } -> std::forward_iterator;
        { std::end(c) }   -> std::forward_iterator;
        { std::begin(const_cast<const C &>(c)) } -> std::forward_iterator;
        { std::end(const_cast<const C &>(c)) }   -> std::forward_iterator;
    };
// clang-format on

template <typename R, typename T>
concept any_convertible_range =
    std::ranges::range<R> &&
    std::convertible_to<std::ranges::range_value_t<R>, T>;

template <typename M, typename K, typename V>
concept any_iterable_mapping =
    std::same_as<typename M::value_type, std::pair<const K, V>> &&
    requires(M m) {
        { std::begin(m) } -> std::forward_iterator;
        { std::end(m) } -> std::forward_iterator;
        { std::begin(const_cast<const M &>(m)) } -> std::forward_iterator;
        { std::end(const_cast<const M &>(m)) } -> std::forward_iterator;
    };

template <typename U, typename T>
concept const_or_rvalue_ref =
    std::convertible_to<U, const T &> || std::convertible_to<U, T &&>;

template <class T>
concept stream_insertable = requires(std::ostream &os, const T &t) { os << t; };

template <class T, class... Matches>
concept same_as_any = (... || std::same_as<T, Matches>);

} // namespace util::concepts

#endif /* end of include guard: CONCEPTS_HPP_0EY957BS */
