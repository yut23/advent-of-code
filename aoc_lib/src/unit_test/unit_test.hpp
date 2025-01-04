#ifndef UNIT_TEST_HPP
#define UNIT_TEST_HPP

#include <algorithm>        // for transform, min
#include <cassert>          // for assert
#include <compare>          // for strong_ordering
#include <concepts>         // for predicate
#include <cstddef>          // for size_t
#include <cstdlib>          // for abs
#include <filesystem>       // for path
#include <functional>       // for function, bind_front
#include <initializer_list> // for initializer_list
#include <iostream> // for cout, cerr, istream, ostream, endl, streambuf, rdbuf, clear
#include <iterator>        // for back_inserter
#include <limits>          // for numeric_limits
#include <source_location> // for source_location
#include <sstream>         // for ostringstream, stringstream
#include <string>          // for string, getline
#include <tuple>           // for tuple, get, make_tuple, apply
#include <type_traits> // for remove_const_t, remove_cvref_t, decay_t, is_same_v, is_assignable_v, is_floating_point_v, is_convertible_v
#include <unordered_map> // for unordered_map, operator==
#include <utility>       // for index_sequence, move, index_sequence_for
#include <vector>        // for vector

#include "unit_test/pretty_print.hpp" // for repr

namespace unit_test {

/**
 * Type trait class for function argument/return type conversion.
 *
 * Should only be accessed through the `arg_lookup_t` helper template and the
 * `convert()` helper function, which sanitize the argument type.
 *
 * If a specific form is not defined, a default will be used. These are defined
 * in detail::defaulted_argument_type_traits.
 */
template <class T>
struct argument_type_traits;

namespace detail {
template <class T>
struct defaulted_argument_type_traits;
}

// Unified interface for looking up converted argument types
template <class T>
using arg_traits =
    detail::defaulted_argument_type_traits<std::remove_cvref_t<T>>;

/**
 * The type defined by the function under test (with an added const qualifier).
 */
struct Argument {};
/**
 * The argument type with cv- and reference-qualifiers stripped.
 */
struct PassedArgument {};
/**
 * The type the user passes to UnitTest::operator().
 */
struct Input {};
/**
 * Stored for the duration of the test; only needed if the argument type
 * requires external memory management.
 */
struct Storage {};
/**
 * Used to compare the actual result of a unit test against the expected result,
 * and to print information about failed tests.
 */
struct Compare {};

template <typename FromSelector, typename ToSelector>
struct convert_tag {};

template <class T, typename Selector>
struct arg_lookup;

template <class T>
struct arg_lookup<T, Argument> {
    using type = typename arg_traits<T>::arg_type;
};
template <class T>
struct arg_lookup<T, PassedArgument> {
    using type = typename arg_traits<T>::passed_arg_type;
};
template <class T>
struct arg_lookup<T, Input> {
    using type = typename arg_traits<T>::input_type;
};
template <class T>
struct arg_lookup<T, Storage> {
    using type = typename arg_traits<T>::storage_type;
};
template <class T>
struct arg_lookup<T, Compare> {
    using type = typename arg_traits<T>::compare_type;
};

template <class T, typename Selector>
using arg_lookup_t = typename arg_lookup<T, Selector>::type;

/**
 * Main type conversion entry point.
 * convert<T, FromSelector, ToSelector>(From &)
 */
template <class T, typename FromSelector, typename ToSelector>
arg_lookup_t<T, ToSelector> convert(arg_lookup_t<T, FromSelector> &x) {
    return arg_traits<T>::convert(convert_tag<FromSelector, ToSelector>{}, x);
}

namespace detail {

template <class T, typename FromSelector, typename ToSelector>
concept has_convert_implementation = requires(arg_lookup_t<T, FromSelector> x) {
    argument_type_traits<T>::convert(convert_tag<FromSelector, ToSelector>{},
                                     x);
};

/**
 * A struct that effectively wraps argument_type_traits and provides defaults
 * for any missing type definitions or convert() overloads.
 */
template <typename T>
struct defaulted_argument_type_traits {
  private:
    // additional traits for selecting the overridden type or a default
    template <class U>
    struct arg_type_traits {
        using type = const U;
    };
    template <class U>
        requires requires { typename argument_type_traits<U>::arg_type; }
    struct arg_type_traits<U> {
        using type = typename argument_type_traits<U>::arg_type;
    };

    template <class U>
    struct passed_arg_type_traits {
        using type = std::decay_t<U>;
    };
    template <class U>
        requires requires { typename argument_type_traits<U>::passed_arg_type; }
    struct passed_arg_type_traits<U> {
        using type = typename argument_type_traits<U>::passed_arg_type;
    };

    template <class U>
    struct input_type_traits {
        using type = const U;
    };
    template <class U>
        requires requires { typename argument_type_traits<U>::input_type; }
    struct input_type_traits<U> {
        using type = typename argument_type_traits<U>::input_type;
    };

    template <class U>
    struct storage_type_traits {
        using type = U;
    };
    template <class U>
        requires requires { typename argument_type_traits<U>::storage_type; }
    struct storage_type_traits<U> {
        using type = typename argument_type_traits<U>::storage_type;
    };

    template <class U>
    struct compare_type_traits {
        using type = U;
    };
    template <class U>
        requires requires { typename argument_type_traits<U>::compare_type; }
    struct compare_type_traits<U> {
        using type = typename argument_type_traits<U>::compare_type;
    };

  public:
    /**
     * The type defined by the function under test (with an added const
     * qualifier).
     */
    using arg_type = typename arg_type_traits<T>::type;
    /**
     * The argument type with cv- and reference-qualifiers stripped.
     */
    using passed_arg_type = typename passed_arg_type_traits<T>::type;
    /**
     * The type the user passes to UnitTest::operator().
     *
     * Must be convertible to `storage_type` and `compare_type`.
     * Must be const-qualified.
     */
    using input_type = typename input_type_traits<T>::type;
    /**
     * Stored for the duration of the test; only needed if the argument type
     * requires external memory management.
     *
     * Must be convertible from `input_type` and to `passed_arg_type`.
     */
    using storage_type = typename storage_type_traits<T>::type;
    /**
     * Used to compare the actual result of a unit test against the expected
     * result, and to print information about failed tests.
     *
     * Must be printable with the stream insertion operator (e.g. `os <<
     * compare_type(...)`).
     * Must be convertible from `arg_type` and `input_type`.
     */
    using compare_type = typename compare_type_traits<T>::type;

    template <typename FromSelector, typename ToSelector>
    static arg_lookup_t<T, ToSelector>
    convert(convert_tag<FromSelector, ToSelector> tag,
            arg_lookup_t<T, FromSelector> &x) {
        if constexpr (has_convert_implementation<T, FromSelector, ToSelector>) {
            return argument_type_traits<T>::convert(tag, x);
        } else {
            static_assert(
                std::is_convertible_v<
                    std::remove_cvref_t<arg_lookup_t<T, FromSelector>>,
                    std::remove_cvref_t<arg_lookup_t<T, ToSelector>>>,
                "missing argument_type_traits::convert() specialization");
            return x;
        }
    }
};

} // namespace detail

/**********************************************************************
 *                argument_type_traits specializations                *
 **********************************************************************/

// vector
template <class T>
struct argument_type_traits<std::vector<T>> {
    using input_type = const std::initializer_list<arg_lookup_t<T, Input>>;
    using storage_type = std::vector<arg_lookup_t<T, Storage>>;
    using compare_type =
        const std::vector<std::remove_const_t<arg_lookup_t<T, Compare>>>;

    template <typename FromSelector, typename ToSelector>
    static arg_lookup_t<std::vector<T>, ToSelector>
    convert(convert_tag<FromSelector, ToSelector>,
            arg_lookup_t<std::vector<T>, FromSelector> &x) {
        std::vector<std::remove_const_t<arg_lookup_t<T, ToSelector>>> result;
        result.reserve(x.size());
        std::transform(
            std::begin(x), std::end(x), std::back_inserter(result),
            [](auto &element) {
                return unit_test::convert<T, FromSelector, ToSelector>(element);
            });
        return result;
    }
};

// tuple-like
namespace detail {
template <template <class...> class TupleLike, class... Ts>
struct tuple_like_arg_traits {
    using input_type = const TupleLike<arg_lookup_t<Ts, Input>...>;
    using storage_type = TupleLike<arg_lookup_t<Ts, Storage>...>;
    using compare_type =
        const TupleLike<std::remove_const_t<arg_lookup_t<Ts, Compare>>...>;

    template <typename FromSelector, typename ToSelector>
    static arg_lookup_t<TupleLike<Ts...>, ToSelector>
    convert(convert_tag<FromSelector, ToSelector>,
            arg_lookup_t<TupleLike<Ts...>, FromSelector> &x) {
        return [&x]<std::size_t... I>(std::index_sequence<I...>)
                   -> arg_lookup_t<TupleLike<Ts...>, ToSelector> {
            return {unit_test::convert<Ts, FromSelector, ToSelector>(
                std::get<I>(x))...};
        }(std::index_sequence_for<Ts...>{});
    }
};
} // namespace detail

// tuple
template <class... Ts>
struct argument_type_traits<std::tuple<Ts...>>
    : public detail::tuple_like_arg_traits<std::tuple, Ts...> {};

// pair
template <class... Ts>
    requires(sizeof...(Ts) == 2)
struct argument_type_traits<std::pair<Ts...>>
    : public detail::tuple_like_arg_traits<std::pair, Ts...> {};

template <class T>
bool test_equality(const T &actual, const T &expected, const int float_ulps) {
    if constexpr (std::is_floating_point_v<T>) {
        return std::abs(actual - expected) <=
               std::abs(expected) * float_ulps *
                   std::numeric_limits<T>::epsilon();
    } else {
        return actual == expected;
    }
}

struct TestResult {
    bool passed;
    std::size_t num_args = 0;
    std::string arguments = "";
    std::string expected = "";
    std::string actual = "";
    std::string output = "";
    std::string note = "";
    // additional lines to print after the test name, number, and arguments
    std::vector<std::string> extra_info{};

    void print(std::ostream &, int test_num) const;
};

void TestResult::print(std::ostream &os, int test_num) const {
    const TestResult &result = *this;
    // Extra info to print after the test name, number, and arguments in case
    // of a failure. Each element should have a final newline.
    std::vector<std::string> extras(extra_info);
    if (!result.actual.empty() || !result.expected.empty()) {
        std::ostringstream ss{};
        ss << "       got: " << result.actual << "\n"
           << "  expected: " << result.expected << "\n";
        extras.push_back(ss.str());
    }
    if (!result.output.empty()) {
        std::ostringstream ss{};
        ss << "  captured output:\n" << result.output;
        extras.push_back(ss.str());
    }
    os << "test case " << test_num;
    if (result.num_args > 0) {
        os << " with " << (result.num_args == 1 ? "input" : "inputs") << " "
           << result.arguments;
    }
    if (result.passed) {
        os << " passed\n";
    } else {
        os << " failed";
        if (!extras.empty() || !result.note.empty()) {
            os << ":";
        }
        if (!result.note.empty()) {
            os << " " << result.note;
        }
        os << "\n";
        for (const std::string &extra : extras) {
            if (!extra.starts_with(" ")) {
                os << " ";
            }
            os << extra;
            if (extra.back() != '\n') {
                os << '\n';
            }
        }
    }
}

short fix_exit_code(std::size_t count) {
    // max return value is 255, but upper 128 are used for signals
    return static_cast<short>(std::min(count, 127UL));
}

/**
 * Base class for test suite types.
 *
 * Handles reporting failed tests, capturing output from the tested code, and
 * computing exit codes.
 *
 * Concrete subclasses should ideally implement operator(), so the user can
 * declare test cases by calling the test object directly.
 */
struct BaseTest {
    std::string name;
    int float_ulps;

    int test_num = 0;
    std::vector<int> failed_tests{};
    bool done_called = false;

  public:
    explicit BaseTest(const std::string &filename, int float_ulps = 2)
        : name(std::filesystem::path(filename).filename()),
          float_ulps(float_ulps) {
        std::cout << name << ": ";
    }

    ~BaseTest() {
        assert(done_called &&
               "error: test.done() not called, exit code will be incorrect");
    }

    int done() {
        done_called = true;
        if (!failed_tests.empty()) {
            std::cout << "  ";
        }
        print_pass_count(std::cout);
        return fix_exit_code(num_failed());
    }

    std::size_t num_failed() const { return failed_tests.size(); }

  protected:
    void record_result(const TestResult &result) {
        if (!result.passed) {
            if (failed_tests.empty()) {
                std::cout << "\n";
            }
            result.print(std::cout, test_num);
            std::cout << std::flush;
            failed_tests.push_back(test_num);
        }
    }

  private:
    void print_pass_count(std::ostream &os) const {
        const int num_passed = test_num - failed_tests.size();
        if (num_passed < test_num) {
            // format in bold red
            os << "\033[1;31m";
        }
        os << num_passed << "/" << test_num;
        if (num_passed < test_num) {
            os << "\033[0m";
        }
        os << " tests passed" << std::endl;
    }
};

/**
 * Helper class that handles executing a test function and constructing a
 * TestResult from it.
 */
template <class R, class... Args>
struct TestRunner {
    using args_tuple = std::tuple<std::remove_cvref_t<Args>...>;
    using func_type = std::function<R(Args...)>;

    TestRunner(func_type func, int float_ulps)
        : function_under_test(func), float_ulps(float_ulps) {}

    TestResult run_on_inputs(arg_lookup_t<Args, Input>... raw_inputs,
                             arg_lookup_t<R, Input> expected,
                             const std::string &note = "") {
        auto inputs = std::make_tuple(raw_inputs...);
        std::remove_const_t<arg_lookup_t<R, Compare>> result_comp =
            [this, &inputs]() {
                auto storages = convert<args_tuple, Input, Storage>(inputs);
                auto args =
                    convert<args_tuple, Storage, PassedArgument>(storages);

                start_capturing_cout();
                auto result = std::apply(function_under_test, args);
                stop_capturing_cout();
                return convert<R, Argument, Compare>(result);
            }();
        return check_result(
            result_comp, expected,
            [&inputs]() { return convert<args_tuple, Input, Compare>(inputs); },
            note);
    }

    TestResult run_on_args(arg_lookup_t<Args, PassedArgument>... raw_args,
                           arg_lookup_t<R, Input> expected,
                           const std::string &note = "") {
        auto args = std::make_tuple(raw_args...);
        start_capturing_cout();
        auto result = std::apply(function_under_test, args);
        stop_capturing_cout();
        auto result_comp = convert<R, Argument, Compare>(result);
        return check_result(
            result_comp, expected,
            [&args]() { return convert<args_tuple, Argument, Compare>(args); },
            note);
    }

  private:
    TestResult check_result(const arg_lookup_t<R, Compare> &result_comp,
                            const arg_lookup_t<R, Input> &expected,
                            std::function<arg_lookup_t<args_tuple, Compare>()>
                                get_args_for_printing,
                            const std::string &note) {
        auto expected_comp = convert<R, Input, Compare>(expected);
        if (!test_equality(result_comp, expected_comp, float_ulps)) {
            std::ostringstream args_ss, expected_ss, actual_ss, output_ss;
            args_ss << pretty_print::repr(get_args_for_printing(), false);
            // add indents to each line of the output
            for (std::string line; std::getline(captured_cout, line);) {
                output_ss << "  # " << line << '\n';
            }
            expected_ss << pretty_print::repr(expected_comp, true);
            actual_ss << pretty_print::repr(result_comp, true);
            return {
                .passed = false,
                .num_args = sizeof...(Args),
                .arguments = args_ss.str(),
                .expected = expected_ss.str(),
                .actual = actual_ss.str(),
                .output = output_ss.str(),
                .note = note,
            };
        } else {
            return {true};
        }
    }

    void start_capturing_cout() {
        assert(old_cout_buf == nullptr &&
               "error: tried to start capturing cout twice in a row");
        // clear the contents
        captured_cout.str("");
        // clear the state flags
        captured_cout.clear();
        old_cout_buf = std::cerr.rdbuf(captured_cout.rdbuf());
    }
    void stop_capturing_cout() {
        assert(old_cout_buf != nullptr &&
               "error: tried to stop capturing cout without starting first");
        std::cerr.rdbuf(old_cout_buf);
        old_cout_buf = nullptr;
    }

    func_type function_under_test;
    int float_ulps;

    std::stringstream captured_cout{};
    std::streambuf *old_cout_buf = nullptr;
};
// template argument deduction guide
template <class R, class... Args>
TestRunner(R (*)(Args...)) -> TestRunner<R, Args...>;

namespace {
template <class C, class R, class... Args,
          template <class, class, class...> class Test, class F,
          typename func_type = typename TestRunner<R, Args...>::func_type>
    requires std::is_assignable_v<func_type, F> ||
             std::is_same_v<F, R (C::*)(Args...)> ||
             std::is_same_v<F, R (C::*)(Args...) const>
func_type _wrap_test_function(Test<C, R, Args...> *test, F func) {
    func_type function_under_test;
    if constexpr (std::is_same_v<F, R (C::*)(Args...)> ||
                  std::is_same_v<F, R (C::*)(Args...) const>) {
        function_under_test = std::bind_front(func, test->obj);
        // function_under_test = [test, func](auto &&...args) {
        //   return
        //   ((test->obj).*func)(std::forward<decltype(args)>(args)...);
        // };
    } else {
        function_under_test = func;
    }
    return function_under_test;
}
} // namespace

// Main unit test class implementation
template <class C, class R, class... Args>
struct MethodTest : public BaseTest {
    using runner_type = TestRunner<R, Args...>;
    using args_tuple = typename runner_type::args_tuple;

    C obj{};
    runner_type runner;

    MethodTest(const std::string &filename, R (C::*func)(Args...) const, C &obj,
               int float_ulps = 2)
        : BaseTest(filename, float_ulps), obj(obj),
          runner(_wrap_test_function(this, func), float_ulps) {}
    MethodTest(const std::string &filename, R (C::*func)(Args...) const,
               C &&obj = {}, int float_ulps = 2)
        : BaseTest(filename, float_ulps), obj(std::move(obj)),
          runner(_wrap_test_function(this, func), float_ulps) {}

    void operator()(arg_lookup_t<Args, Input>... raw_inputs,
                    arg_lookup_t<R, Input> expected,
                    const std::string &note = "") {
        ++test_num;
        record_result(runner.run_on_inputs(raw_inputs..., expected, note));
    }

    /// Use this when programmatically generating tests, since
    /// std::initializer_lists can't be constructed at runtime.
    void run_on_args(arg_lookup_t<Args, PassedArgument>... raw_args,
                     arg_lookup_t<R, Input> expected,
                     const std::string &note = "") {
        ++test_num;
        record_result(runner.run_on_args(raw_args..., expected, note));
    }
};

// explicit template argument deduction guides
template <class C, class R, class... Args>
MethodTest(const std::string &, R (C::*)(Args...)) -> MethodTest<C, R, Args...>;
template <class C, class R, class... Args>
MethodTest(const std::string &, R (C::*)(Args...), C &&)
    -> MethodTest<C, R, Args...>;
template <class C, class R, class... Args>
MethodTest(const std::string &, R (C::*)(Args...), C &&, int)
    -> MethodTest<C, R, Args...>;

/// Test class for pure functions.
template <class R, class... Args>
struct PureTest : public BaseTest {
    using runner_type = TestRunner<R, Args...>;
    using args_tuple = typename runner_type::args_tuple;

    runner_type runner;

    PureTest(const std::string &filename, typename runner_type::func_type func,
             int float_ulps = 2)
        : BaseTest(filename, float_ulps), runner(func, float_ulps) {}

    void operator()(arg_lookup_t<Args, Input>... raw_inputs,
                    arg_lookup_t<R, Input> expected,
                    const std::string &note = "") {
        ++test_num;
        record_result(runner.run_on_inputs(raw_inputs..., expected, note));
    }

    /// Use this when programmatically generating tests, since
    /// std::initializer_lists can't be constructed at runtime.
    void run_on_args(arg_lookup_t<Args, PassedArgument>... raw_args,
                     arg_lookup_t<R, Input> expected,
                     const std::string &note = "") {
        ++test_num;
        record_result(runner.run_on_args(raw_args..., expected, note));
    }
};

template <class R, class... Args>
PureTest(const std::string &, R (*)(Args...)) -> PureTest<R, Args...>;
template <class R, class... Args>
PureTest(const std::string &, R (*)(Args...), int) -> PureTest<R, Args...>;
template <class R, class... Args>
PureTest(const std::string &, std::function<R(Args...)>)
    -> PureTest<R, Args...>;
template <class R, class... Args>
PureTest(const std::string &, std::function<R(Args...)>, int)
    -> PureTest<R, Args...>;

// A collection of test cases, xUnit style
struct TestSuite : public BaseTest {
    explicit TestSuite(const std::string &name) : BaseTest(name) {}

    void test(const std::string &test_name, auto &&test_func) {
        // run a test function, catching any check failures
        ++test_num;
        try {
            test_func();
        } catch (TestResult &result) {
            result.note = test_name;
            record_result(result);
            return;
        }
        record_result(TestResult{true});
    }
};

// checker functions to be used in TestSuite cases
namespace checks {
using SL = std::source_location;

namespace detail {
TestResult failure_helper(const SL loc, const std::string &info,
                          const std::string &check_type = "check") {
    TestResult result;
    result.passed = false;
    std::ostringstream msg_ss{};
    msg_ss << loc.file_name() << ":" << loc.line() << ":" << loc.column()
           << ": failed " << check_type;
    if (!info.empty()) {
        msg_ss << ": " << info;
    }
    result.extra_info.push_back(msg_ss.str());
    return result;
}

TestResult equal_failure_helper(const auto &actual, const auto &expected,
                                const SL loc, const std::string &info) {
    TestResult result = failure_helper(loc, info, "equality check");
    if constexpr (pretty_print::has_print_repr<
                      std::remove_cvref_t<decltype(actual)>> &&
                  pretty_print::has_print_repr<
                      std::remove_cvref_t<decltype(expected)>>) {
        // we can pretty-print the values being compared
        std::ostringstream actual_ss, expected_ss;
        actual_ss << pretty_print::repr(actual, true);
        result.actual = actual_ss.str();
        expected_ss << pretty_print::repr(expected, true);
        result.expected = expected_ss.str();
    }
    return result;
}
} // namespace detail

void check(auto &&condition, const std::string &info = "",
           const SL loc = SL::current()) {
    if (!condition) {
        throw detail::failure_helper(loc, info);
    }
}

void check(auto &&condition,
           std::invocable<std::ostream &> auto &&info_callback,
           const SL loc = SL::current()) {
    if (!condition) {
        std::ostringstream info_ss;
        info_callback(info_ss);
        throw detail::failure_helper(loc, info_ss.str());
    }
}

void check_equal(const auto &actual, const auto &expected,
                 const std::string &info = "", const SL loc = SL::current()) {
    if (actual != expected) {
        throw detail::equal_failure_helper(actual, expected, loc, info);
    }
}

void check_equal(const auto &actual, const auto &expected,
                 std::invocable<std::ostream &> auto &&info,
                 const SL loc = SL::current()) {
    if (actual != expected) {
        std::ostringstream info_ss;
        info(info_ss);
        throw detail::equal_failure_helper(actual, expected, loc,
                                           info_ss.str());
    }
}
} // namespace checks

namespace {
[[maybe_unused]] void _lint_helper() {
    {
        unit_test::PureTest test(
            "foobar",
            +[](const std::vector<int> &vec1, const std::vector<int> &vec2)
                -> std::strong_ordering { return vec1 <=> vec2; });
        test({1, 2}, {1, 2}, std::strong_ordering::equal);
        test({1, 1}, {1, 2}, std::strong_ordering::less);
        test({2, 2}, {1, 2}, std::strong_ordering::greater);
        test({1, 2, 3}, {1, 2}, std::strong_ordering::greater);
        test({1, 1, 3}, {1, 2}, std::strong_ordering::less, "note");
        test.done();
    }
    {
        const auto count_chars =
            +[](const std::string &s) -> std::unordered_map<char, int> {
            std::unordered_map<char, int> counts;
            for (char ch : s) {
                ++counts[ch];
            }
            return counts;
        };
        unit_test::PureTest test("count_chars", count_chars);
        test("abc", {{'a', 1}, {'b', 1}, {'c', 1}});
        test("banana", {{'a', 3}, {'b', 1}, {'n', 2}});
        test.done();
    }
}
} // namespace

} // namespace unit_test

#endif /* end of include guard: UNIT_TEST_HPP */
