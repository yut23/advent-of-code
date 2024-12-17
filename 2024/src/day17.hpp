/******************************************************************************
 * File:        day17.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-17
 *****************************************************************************/

#ifndef DAY17_HPP_AEORLLPV
#define DAY17_HPP_AEORLLPV

#include "lib.hpp" // for skip
#include "unit_test/pretty_print.hpp"
#include <cassert>  // for assert
#include <cstddef>  // for size_t
#include <iostream> // for istream
#include <string>   // for string, getline
#include <vector>   // for vector

namespace aoc::day17 {

enum class Opcode : char {
    adv = 0, // A := A / pow(2, combo operand) === A >> combo operand
    bxl = 1, // B := B ^ literal operand
    bst = 2, // B := combo operand % 8
    jnz = 3, // set IP to literal operand if A != 0
    bxc = 4, // B := B ^ C
    out = 5, // output combo operand % 8
    bdv = 6, // B := A >> combo operand
    cdv = 7, // C := C >> combo operand
};

class ChronoComputer {
    std::vector<short> program{};
    std::size_t ip = 0;
    int A = 0, B = 0, C = 0;

  private:
    int literal_operand() const { return program[ip + 1]; }
    int combo_operand() const {
        short value = program[ip + 1];
        switch (value) {
        case 0:
        case 1:
        case 2:
        case 3:
            return value;
        case 4:
            return A;
        case 5:
            return B;
        case 6:
            return C;
        }
        assert(false);
    }

    ChronoComputer() = default;

  public:
    std::vector<short> execute();

    static ChronoComputer read(std::istream &);
    friend std::ostream &operator<<(std::ostream &, const ChronoComputer &);
};

std::vector<short> ChronoComputer::execute() {
    std::vector<short> output;

    while (ip < program.size()) {
        if constexpr (aoc::DEBUG) {
            std::cerr << program[ip] << "," << program[ip + 1] << ": ";
        }
        switch (Opcode(program[ip])) {
        case Opcode::adv:
            if constexpr (aoc::DEBUG) {
                std::cerr << "adv " << program[ip + 1]
                          << ": setting register A to " << A << " / 2^"
                          << combo_operand() << " = ";
            }
            A = A >> combo_operand();
            if constexpr (aoc::DEBUG) {
                std::cerr << A << "\n";
            }
            break;
        case Opcode::bdv:
            if constexpr (aoc::DEBUG) {
                std::cerr << "bdv " << program[ip + 1]
                          << ": setting register B to " << A << " / 2^"
                          << combo_operand() << " = ";
            }
            B = A >> combo_operand();
            if constexpr (aoc::DEBUG) {
                std::cerr << B << "\n";
            }
            break;
        case Opcode::cdv:
            if constexpr (aoc::DEBUG) {
                std::cerr << "cdv " << program[ip + 1]
                          << ": setting register C to " << A << " / 2^"
                          << combo_operand() << " = ";
            }
            C = A >> combo_operand();
            if constexpr (aoc::DEBUG) {
                std::cerr << C << "\n";
            }
            break;
        case Opcode::bxl:
            if constexpr (aoc::DEBUG) {
                std::cerr << "bxl " << program[ip + 1]
                          << ": setting register B to " << B << " ^ "
                          << literal_operand() << " = ";
            }
            B = B ^ literal_operand();
            if constexpr (aoc::DEBUG) {
                std::cerr << B << "\n";
            }
            break;
        case Opcode::bxc:
            if constexpr (aoc::DEBUG) {
                std::cerr << "bxc: setting register B to " << B << " ^ " << C
                          << " = ";
            }
            B = B ^ C;
            if constexpr (aoc::DEBUG) {
                std::cerr << B << "\n";
            }
            break;
        case Opcode::bst:
            if constexpr (aoc::DEBUG) {
                std::cerr << "bst " << program[ip + 1]
                          << ": setting register B to " << combo_operand()
                          << " % 8 = ";
            }
            B = combo_operand() % 8;
            if constexpr (aoc::DEBUG) {
                std::cerr << B << "\n";
            }
            break;
        case Opcode::out:
            if constexpr (aoc::DEBUG) {
                std::cerr << "out " << program[ip + 1] << ": outputting "
                          << combo_operand() << " % 8 = ";
            }
            output.push_back(combo_operand() % 8);
            if constexpr (aoc::DEBUG) {
                std::cerr << output.back() << "\n";
            }
            break;
        case Opcode::jnz:
            if constexpr (aoc::DEBUG) {
                std::cerr << "jnz " << program[ip + 1] << ": A = " << A << ", ";
            }
            if (A != 0) {
                // subtract 2 to compensate for increment below
                ip = literal_operand() - 2;
                if constexpr (aoc::DEBUG) {
                    std::cerr << "jumping to " << ip + 2 << "\n";
                }
            } else {
                if constexpr (aoc::DEBUG) {
                    std::cerr << "not jumping\n";
                }
            }
            break;
        }
        ip += 2;
    }

    return output;
}

ChronoComputer ChronoComputer::read(std::istream &is) {
    ChronoComputer computer;
    is >> aoc::skip<std::string>(2) >> computer.A;
    is >> aoc::skip<std::string>(2) >> computer.B;
    is >> aoc::skip<std::string>(2) >> computer.C;
    is >> std::ws;
    is >> aoc::skip<std::string>(1);

    unsigned char ch;
    while (is >> ch) {
        computer.program.push_back(ch - '0');
        if (is >> ch) {
            assert(ch == ',');
        }
    }
    if constexpr (aoc::DEBUG) {
        std::cerr << computer << "\n";
    }
    return computer;
}

std::ostream &operator<<(std::ostream &os, const ChronoComputer &computer) {
    os << "IP = " << computer.ip << "\n"
       << "A  = " << computer.A << "\n"
       << "B  = " << computer.B << "\n"
       << "C  = " << computer.C << "\n"
       << "program: " << pretty_print::repr(computer.program) << "\n";
    return os;
}

} // namespace aoc::day17

#endif /* end of include guard: DAY17_HPP_AEORLLPV */
