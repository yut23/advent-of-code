/******************************************************************************
 * File:        test21.cpp
 *
 * Author:      Eric T. Johnson (yut23)
 * Created:     2025-01-28
 *****************************************************************************/

#include "day21.hpp" // IWYU pragma: associated

#include "unit_test/unit_test.hpp"

#include <cstddef> // for size_t
#include <sstream> // for istringstream

namespace aoc::day21::test {

std::vector<Key> operator""_k(const char *chars, std::size_t len) {
    std::istringstream iss(std::string{chars, len});
    return aoc::read_vector<Key>(iss);
}

std::size_t test_move_arm() {
    unit_test::PureTest test(
        "aoc::day21::move_arm", +[](Key from, Key to) {
            std::vector<Key> outputs;
            move_arm(KEY_POSITIONS.at(from), KEY_POSITIONS.at(to), outputs);
            return outputs;
        });

    test(Key::ACTIVATE, Key(9), "^^^"_k);
    test(Key::ACTIVATE, Key(4), "^^<<"_k);
    test(Key(4), Key::ACTIVATE, ">>vv"_k);
    test(Key(3), Key(7), "<<^^"_k);
    test(Key(7), Key(3), "vv>>"_k);
    test(Key(1), Key(1), ""_k);

    return test.done(), test.num_failed();
}

std::size_t test_control_arm() {
    unit_test::PureTest test("aoc::day21::control_arm", &control_arm);

    test("^"_k, "<A"_k);
    test(">"_k, "vA"_k);
    test("v"_k, "<vA"_k);
    test("<"_k, "v<<A"_k);

    test("^A"_k, "<A>A"_k);
    test(">A"_k, "vA^A"_k);
    test("vA"_k, "<vA^>A"_k);
    test("<A"_k, "v<<A>>^A"_k);

    test("^v"_k, "<AvA"_k);
    test("^<"_k, "<Av<A"_k);

    test("^^^"_k, "<AAA"_k);
    test("<<>>^A>"_k, "v<<AA>>AA<^A>AvA"_k);

    return test.done(), test.num_failed();
};

struct DebugController : public RobotController {
    std::vector<std::vector<Key>> key_presses{};

    explicit DebugController(int layers_)
        : RobotController(layers_),
          key_presses(layers + 1, std::vector<Key>{}) {}

  protected:
    virtual void on_keypress(Key key, int layer) override {
        key_presses[layer].push_back(key);
    }
};

std::size_t test_press_key_debug() {
    unit_test::PureTest test(
        "aoc::day21::press_key debug", +[](int layers, Key key) {
            DebugController controller(layers);
            controller.press_key(key);
            return controller.key_presses[0];
        });

    using enum Key;
    test(0, UP, "^"_k);
    test(1, UP, "<A"_k);
    test(2, UP, "v<<A>>^A"_k);

    test(0, DOWN, "v"_k);
    test(1, DOWN, "<vA"_k);
    test(2, DOWN, "v<<A>A^>A"_k);

    test(0, RIGHT, ">"_k);
    test(1, RIGHT, "vA"_k);
    test(2, RIGHT, "<vA^>A"_k);

    test(0, LEFT, "<"_k);
    test(1, LEFT, "v<<A"_k);
    test(2, LEFT, "<vA<AA>>^A"_k);

    test(0, ACTIVATE, "A"_k);
    test(1, ACTIVATE, "A"_k);
    test(2, ACTIVATE, "A"_k);
    test(3, ACTIVATE, "A"_k);
    test(4, ACTIVATE, "A"_k);

    test(0, ONE, "1"_k);
    test(1, ONE, "^<<A"_k);
    test(2, ONE, "<Av<AA>>^A"_k);

    test(0, EIGHT, "8"_k);
    test(1, EIGHT, "<^^^A"_k);
    test(2, EIGHT, "v<<A>^AAA>A"_k);
    test(3, EIGHT, "<vA<AA>>^AvA<^A>AAAvA^A"_k);

    return test.done(), test.num_failed();
};

std::size_t test_press_key_count() {
    unit_test::PureTest test(
        "aoc::day21::press_key counts", +[](int layers, Key key) {
            RobotController controller(layers);
            return controller.press_key(key);
        });

    using enum Key;
    test(0, UP, "^"_k.size());
    test(1, UP, "<A"_k.size());
    test(2, UP, "v<<A>>^A"_k.size());

    test(0, DOWN, "v"_k.size());
    test(1, DOWN, "<vA"_k.size());
    test(2, DOWN, "v<<A>A^>A"_k.size());

    test(0, RIGHT, ">"_k.size());
    test(1, RIGHT, "vA"_k.size());
    test(2, RIGHT, "<vA^>A"_k.size());

    test(0, LEFT, "<"_k.size());
    test(1, LEFT, "v<<A"_k.size());
    test(2, LEFT, "<vA<AA>>^A"_k.size());

    test(0, ACTIVATE, "A"_k.size());
    test(1, ACTIVATE, "A"_k.size());
    test(2, ACTIVATE, "A"_k.size());
    test(3, ACTIVATE, "A"_k.size());
    test(4, ACTIVATE, "A"_k.size());

    test(0, ONE, "1"_k.size());
    test(1, ONE, "^<<A"_k.size());
    test(2, ONE, "<Av<AA>>^A"_k.size());

    test(0, EIGHT, "8"_k.size());
    test(1, EIGHT, "<^^^A"_k.size());
    test(2, EIGHT, "v<<A>^AAA>A"_k.size());
    test(3, EIGHT, "<vA<AA>>^AvA<^A>AAAvA^A"_k.size());

    return test.done(), test.num_failed();
};

std::size_t test_count_presses_dfs() {
    unit_test::PureTest test("aoc::day21::count_presses_dfs",
                             &count_presses_dfs);

    test("029A"_k, 0, 4);
    test("029A"_k, 1, 12);
    test("029A"_k, 2, 28);
    test("029A"_k, 3, 68);
    test("029A"_k, 4, 164);
    test("029A"_k, 5, 404);

    test("127A"_k, 0, 4);
    test("127A"_k, 1, 16);
    test("127A"_k, 2, 34);
    test("127A"_k, 3, 82);
    test("127A"_k, 4, 204);
    test("127A"_k, 5, 494);

    test("084A"_k, 0, 4);
    test("084A"_k, 1, 14);
    test("084A"_k, 2, 32);
    test("084A"_k, 3, 76);
    test("084A"_k, 4, 186);
    test("084A"_k, 5, 454);

    return test.done(), test.num_failed();
}

std::size_t test_count_presses_memo() {
    unit_test::PureTest test("aoc::day21::count_presses_memo",
                             &count_presses_memo);

    test("029A"_k, 0, 4);
    test("029A"_k, 1, 12);
    test("029A"_k, 2, 28);
    test("029A"_k, 3, 68);
    test("029A"_k, 4, 164);
    test("029A"_k, 5, 404);

    test("127A"_k, 0, 4);
    test("127A"_k, 1, 16);
    test("127A"_k, 2, 34);
    test("127A"_k, 3, 82);
    test("127A"_k, 4, 204);
    test("127A"_k, 5, 494);

    test("084A"_k, 0, 4);
    test("084A"_k, 1, 14);
    test("084A"_k, 2, 32);
    test("084A"_k, 3, 76);
    test("084A"_k, 4, 186);
    test("084A"_k, 5, 454);

    return test.done(), test.num_failed();
}

} // namespace aoc::day21::test

int main() {
    std::size_t num_failed = 0;
    num_failed += aoc::day21::test::test_move_arm();
    num_failed += aoc::day21::test::test_control_arm();
    num_failed += aoc::day21::test::test_press_key_debug();
    num_failed += aoc::day21::test::test_press_key_count();
    num_failed += aoc::day21::test::test_count_presses_dfs();
    num_failed += aoc::day21::test::test_count_presses_memo();
    return unit_test::fix_exit_code(num_failed);
}
