/******************************************************************************
 * File:        day22.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-22
 *****************************************************************************/

#ifndef DAY22_HPP_A3Y597BB
#define DAY22_HPP_A3Y597BB

#include <iostream> // for istream
#include <string>   // for string, getline

namespace aoc::day22 {

void xorshift(long &secret, int shift) {
    long tmp = secret;
    if (shift < 0) {
        tmp >>= -shift;
    } else {
        tmp <<= shift;
    }
    secret = (secret ^ tmp) & 0xFF'FF'FF;
}

void evolve_secret(long &secret) {
    xorshift(secret, +6);
    xorshift(secret, -5);
    xorshift(secret, +11);
}

} // namespace aoc::day22

#endif /* end of include guard: DAY22_HPP_A3Y597BB */
