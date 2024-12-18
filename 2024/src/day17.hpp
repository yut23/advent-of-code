/******************************************************************************
 * File:        day17.hpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2024-12-17
 *****************************************************************************/

#ifndef DAY17_HPP_AEORLLPV
#define DAY17_HPP_AEORLLPV

#include "lib.hpp"   // for as_number, skip, DEBUG
#include <algorithm> // for min_element
#include <cassert>   // for assert
#include <cstddef>   // for size_t
#include <cstdint>   // for uint64_t, int8_t
#include <iostream>  // for istream, ws, cerr, dec, oct
#include <optional>  // for optional
#include <set>       // for set
#include <sstream>   // for ostringstream
#include <string>    // for string
#include <utility>   // for swap, pair
#include <vector>    // for vector

namespace aoc::day17 {

using code_t = std::int8_t;

enum class Opcode : code_t {
    adv = 0, // A := A / pow(2, combo operand) === A >> combo operand
    bxl = 1, // B := B ^ literal operand
    bst = 2, // B := combo operand % 8
    jnz = 3, // set IP to literal operand if A != 0
    bxc = 4, // B := B ^ C
    out = 5, // output combo operand % 8
    bdv = 6, // B := A >> combo operand
    cdv = 7, // C := C >> combo operand
};

enum RegisterMask : char {
    NO_REGISTER = 0,
    REGISTER_A = 1,
    REGISTER_B = 2,
    REGISTER_C = 4,
};
struct Instruction {
    Opcode opcode;
    code_t arg;
    RegisterMask reads = NO_REGISTER;
    RegisterMask writes = NO_REGISTER;

    Instruction(Opcode opcode, code_t arg);
    Instruction(code_t opcode, code_t arg) : Instruction(Opcode(opcode), arg) {}
};

Instruction::Instruction(Opcode opcode, code_t arg) : opcode(opcode), arg(arg) {
    RegisterMask combo_register = NO_REGISTER;
    if (arg == 4) {
        combo_register = REGISTER_A;
    } else if (arg == 5) {
        combo_register = REGISTER_B;
    } else if (arg == 6) {
        combo_register = REGISTER_C;
    }
    switch (opcode) {
    case Opcode::adv:
        reads = static_cast<RegisterMask>(REGISTER_A | combo_register);
        writes = REGISTER_A;
        break;
    case Opcode::bdv:
        reads = static_cast<RegisterMask>(REGISTER_A | combo_register);
        writes = REGISTER_B;
        break;
    case Opcode::cdv:
        reads = static_cast<RegisterMask>(REGISTER_A | combo_register);
        writes = REGISTER_C;
        break;
    case Opcode::bxl:
        reads = writes = REGISTER_B;
        break;
    case Opcode::bst:
        reads = combo_register;
        writes = REGISTER_B;
        break;
    case Opcode::jnz:
        reads = REGISTER_A;
        break;
    case Opcode::bxc:
        reads = static_cast<RegisterMask>(REGISTER_B | REGISTER_C);
        writes = REGISTER_B;
        break;
    case Opcode::out:
        reads = combo_register;
        break;
    }
}

struct Registers {
    std::uint64_t A;
    std::uint64_t B;
    std::uint64_t C;

    std::uint64_t combo_operand(code_t arg) const {
        switch (arg) {
        case 0:
        case 1:
        case 2:
        case 3:
            return arg;
        case 4:
            return A;
        case 5:
            return B;
        case 6:
            return C;
        }
        assert(false);
    }
};

template <bool verbose = aoc::DEBUG, class OutputHandler>
    requires(requires(OutputHandler handler, code_t value) { handler(value); })
void run_program(const std::vector<code_t> &program, Registers regs,
                 OutputHandler &&output_handler) {
    std::size_t ip = 0;
    while (ip < program.size()) {
        if constexpr (verbose) {
            std::cerr << aoc::as_number(program[ip]) << ","
                      << aoc::as_number(program[ip + 1]) << ": ";
        }
        int arg = program[ip + 1];
        switch (Opcode(program[ip])) {
        case Opcode::adv:
            if constexpr (verbose) {
                std::cerr << "adv " << aoc::as_number(program[ip + 1])
                          << ": setting register A to " << regs.A << " >> "
                          << regs.combo_operand(arg) << " = ";
            }
            regs.A = regs.A >> regs.combo_operand(arg);
            if constexpr (verbose) {
                std::cerr << regs.A << "\n";
            }
            break;
        case Opcode::bdv:
            if constexpr (verbose) {
                std::cerr << "bdv " << aoc::as_number(program[ip + 1])
                          << ": setting register B to " << regs.A << " >> "
                          << regs.combo_operand(arg) << " = ";
            }
            regs.B = regs.A >> regs.combo_operand(arg);
            if constexpr (verbose) {
                std::cerr << regs.B << "\n";
            }
            break;
        case Opcode::cdv:
            if constexpr (verbose) {
                std::cerr << "cdv " << aoc::as_number(program[ip + 1])
                          << ": setting register C to " << regs.A << " >> "
                          << regs.combo_operand(arg) << " = ";
            }
            regs.C = regs.A >> regs.combo_operand(arg);
            if constexpr (verbose) {
                std::cerr << regs.C << "\n";
            }
            break;
        case Opcode::bxl:
            if constexpr (verbose) {
                std::cerr << "bxl " << aoc::as_number(program[ip + 1])
                          << ": setting register B to " << regs.B << " ^ "
                          << arg << " = ";
            }
            regs.B = regs.B ^ arg;
            if constexpr (verbose) {
                std::cerr << regs.B << "\n";
            }
            break;
        case Opcode::bxc:
            if constexpr (verbose) {
                std::cerr << "bxc: setting register B to " << regs.B << " ^ "
                          << regs.C << " = ";
            }
            regs.B = regs.B ^ regs.C;
            if constexpr (verbose) {
                std::cerr << regs.B << "\n";
            }
            break;
        case Opcode::bst:
            if constexpr (verbose) {
                std::cerr << "bst " << aoc::as_number(program[ip + 1])
                          << ": setting register B to "
                          << regs.combo_operand(arg) << " % 8 = ";
            }
            regs.B = regs.combo_operand(arg) & 0b111;
            if constexpr (verbose) {
                std::cerr << regs.B << "\n";
            }
            break;
        case Opcode::out: {
            code_t tmp = regs.combo_operand(arg) & 0b111;
            if constexpr (verbose) {
                std::cerr << "out " << aoc::as_number(program[ip + 1])
                          << ": outputting " << regs.combo_operand(arg)
                          << " % 8 = " << aoc::as_number(tmp) << "\n";
            }
            output_handler(tmp);
            break;
        }
        case Opcode::jnz:
            if constexpr (verbose) {
                std::cerr << "jnz " << aoc::as_number(program[ip + 1])
                          << ": A = " << regs.A << ", ";
            }
            if (regs.A != 0) {
                ip = arg;
                if constexpr (verbose) {
                    std::cerr << "jumping to " << ip << "\n";
                }
                // don't increment the ip
                continue;
            } else {
                if constexpr (verbose) {
                    std::cerr << "not jumping\n";
                }
            }
            break;
        }
        ip += 2;
    }
}

/**
 * Returns an empty optional if the assumptions for solve_quine() are valid, or
 * the reason if not.
 *
 * Assumptions: program contains exactly one `adv 3` and one `out`, ends with a
 * `jnz 0`, and doesn't carry the values of B or C between iterations. This
 * means that each loop iteration processes the current value of A, then shifts
 * it right by 3 bits (1 octal digit).
 */
std::optional<std::string>
is_possible_quine(const std::vector<code_t> &program) {
    std::ostringstream oss{};
    // must contain adv, out, and jnz at the very least
    if (program.size() < 6) {
        return "program too small";
    }
    bool found_adv = false, found_out = false, found_jnz = false;
    bool assigned_B = false, assigned_C = false;
    for (std::size_t i = 0; i + 1 < program.size(); i += 2) {
        Instruction inst(program[i], program[i + 1]);
        if ((inst.reads & REGISTER_B) == REGISTER_B && !assigned_B) {
            oss << "instruction at " << i
                << " uses B from the previous loop iteration";
            return oss.str();
        }
        if ((inst.reads & REGISTER_C) == REGISTER_C && !assigned_C) {
            oss << "instruction at " << i
                << " uses C from the previous loop iteration";
            return oss.str();
        }
        if (inst.opcode == Opcode::adv) {
            if (inst.arg != 3) {
                oss << "bad operand for adv at " << i << ": expected 3, got "
                    << aoc::as_number(inst.arg);
                return oss.str();
            }
            if (found_adv) {
                return "multiple adv instructions found";
            }
            found_adv = true;
        } else if (inst.opcode == Opcode::out) {
            if (found_out) {
                return "multiple out instructions found";
            }
            found_out = true;
        } else if (inst.opcode == Opcode::jnz) {
            if (inst.arg != 0) {
                oss << "bad operand for jnz at " << i << ": expected 0, got "
                    << aoc::as_number(inst.arg);
                return oss.str();
            }
            if (i != program.size() - 2) {
                oss << "jnz found before the end of the program, at offset "
                    << i;
                return oss.str();
            }
            found_jnz = true;
        }
        if ((inst.writes & REGISTER_B) == REGISTER_B) {
            assigned_B = true;
        }
        if ((inst.writes & REGISTER_C) == REGISTER_C) {
            assigned_C = true;
        }
    }
    if (!found_adv) {
        return "no adv instruction found";
    }
    if (!found_out) {
        return "no out instruction found";
    }
    if (!found_jnz) {
        return "no jnz instruction found";
    }
    return {};
}

/*
 * Approach: Start from the last output digit, which corresponds to the
 * most-significant digit of A, as the loop has to terminate after that digit.
 */
std::uint64_t solve_quine(const std::vector<code_t> &orig_program) {
    // Remove the jnz instruction from the end of the program, so we can try
    // each iteration on each possible digit of A independently
    std::vector<code_t> program(orig_program.begin(), orig_program.end() - 2);
    // B and C are assigned before use, so their values don't matter
    Registers regs{0, 0, 0};

    if constexpr (aoc::DEBUG) {
        std::cerr << std::oct;
    }
    // all possible values for A we've found so far
    std::set<std::uint64_t> curr_values = {0};
    // new valid values for A
    std::set<std::uint64_t> new_values;
    // loop over output digits in reverse
    for (auto it = orig_program.rbegin(); it != orig_program.rend(); ++it) {
        for (std::uint64_t curr_A : curr_values) {
            // if constexpr (aoc::DEBUG) {
            //     std::cerr << "adding octal digits to A=" << curr_A << " to
            //     get "
            //               << aoc::as_number(*it) << "...\n";
            // }
            //  try adding each octal digit to each possible value of A
            for (std::uint64_t new_digit = 0; new_digit < 8; ++new_digit) {
                code_t result = *it ^ 1;
                regs.A = (curr_A << 3) | new_digit;
                constexpr bool verbose = false;
                run_program<verbose>(
                    program, regs, [&result](code_t value) { result = value; });
                if (result == *it) {
                    // if constexpr (aoc::DEBUG) {
                    //     std::cerr << "  found working digit: A=" << regs.A
                    //               << "\n";
                    // }
                    new_values.insert(regs.A);
                }
            }
        }
        curr_values.clear();
        std::swap(curr_values, new_values);
        new_values.clear();
        if constexpr (aoc::DEBUG) {
            std::cerr << curr_values.size() << ", ";
        }
    }
    if constexpr (aoc::DEBUG) {
        std::cerr << "\nfound " << curr_values.size() << " quine inputs\n";
        std::cerr << std::dec;
    }
    return *std::min_element(curr_values.begin(), curr_values.end());
}

std::pair<std::vector<code_t>, Registers> read_input(std::istream &is) {
    Registers regs;
    is >> aoc::skip<std::string>(2) >> regs.A;
    is >> aoc::skip<std::string>(2) >> regs.B;
    is >> aoc::skip<std::string>(2) >> regs.C;
    is >> std::ws;
    is >> aoc::skip<std::string>(1);

    unsigned char ch;
    std::vector<code_t> program;
    while (is >> ch) {
        program.push_back(ch - '0');
        if (is >> ch) {
            assert(ch == ',');
        }
    }
    if constexpr (aoc::DEBUG) {
        std::cerr << "A = " << regs.A << "\n"
                  << "B = " << regs.B << "\n"
                  << "C = " << regs.C << "\n"
                  << "program: ";
        for (std::size_t i = 0; i < program.size(); ++i) {
            if (i > 0) {
                std::cerr << ",";
            }
            std::cerr << aoc::as_number(program[i]);
        }
        std::cerr << "\n\n";
    }
    return {program, regs};
}

} // namespace aoc::day17

#endif /* end of include guard: DAY17_HPP_AEORLLPV */
