#ifndef UNIT_TEST_HPP
#define UNIT_TEST_HPP

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <filesystem>
#include <functional>
#include <iostream>
#include <set>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <vector>

#include "pretty_print.hpp"
#include "util.hpp"

namespace unit_test {

template <typename T>
struct argument_traits;
// Unified interface for looking up converted argument types
template <class T>
using arg_traits = argument_traits<std::remove_cvref_t<T>>;

/// Same as orig_arg_type, but with a const qualifier.
struct Argument {};
/// The argument type with cv- and reference-qualifiers stripped.
struct PassedArgument {};
/// The type the user passes to UnitTest::operator().
struct Input {};
/// Stored for the duration of the test; only needed if the argument type
/// requires external memory management.
struct Storage {};
/// Used to compare the actual result of a unit test against the expected
/// result, and to print information about failed tests.
struct Compare {};

template <class T, typename Selector>
struct arg_lookup;

template <class T>
struct arg_lookup<T, Argument> {
    typedef typename arg_traits<T>::arg_type type;
};
template <class T>
struct arg_lookup<T, PassedArgument> {
    typedef typename arg_traits<T>::passed_arg_type type;
};
template <class T>
struct arg_lookup<T, Input> {
    typedef const typename arg_traits<T>::input_type type;
};
template <class T>
struct arg_lookup<T, Storage> {
    typedef typename arg_traits<T>::storage_type type;
};
template <class T>
struct arg_lookup<T, Compare> {
    typedef typename arg_traits<T>::compare_type type;
};

template <class T, typename Selector>
using arg_lookup_t = typename arg_lookup<T, Selector>::type;

namespace detail {
/**
 * A class template for adapting function argument/return types between their
 * input, storage, argument, and comparison forms.
 */
template <class T>
struct base_argument_traits {
    /**
     * The type defined by the function under test.
     */
    using orig_arg_type = T;
    /**
     * Same as orig_arg_type, but with a const qualifier.
     */
    using arg_type = const T;
    /**
     * The argument type with cv- and reference-qualifiers stripped.
     */
    using passed_arg_type = std::decay_t<T>;
    /**
     * The type the user passes to UnitTest::operator().
     *
     * Must be convertible to `storage_type` and `compare_type`.
     * Must be const-qualified.
     */
    using input_type = const T;
    /**
     * Stored for the duration of the test; only needed if the argument type
     * requires external memory management.
     *
     * Must be convertible from `input_type` and to `passed_arg_type`.
     */
    using storage_type = T;
    /**
     * Used to compare the actual result of a unit test against the expected
     * result, and to print information about failed tests.
     *
     * Must be convertible from `arg_type` and `input_type`.
     */
    using compare_type = T;

    static storage_type input_to_storage(const input_type &input) {
        return input;
    }
    static passed_arg_type storage_to_arg(const storage_type &storage) {
        return storage;
    }
    /**
     * Convert an argument type to a comparison type.
     *
     * Only used on the actual result of the unit test.
     */
    static compare_type arg_to_compare(const arg_type &arg) { return arg; }
    /**
     * Convert an input type to a comparison type.
     *
     * Used on the expected result passed by the user and on the input arguments
     * if a test fails.
     */
    static compare_type input_to_compare(const input_type &input) {
        return input;
    }
};

// Conversion dispatchers
template <typename...>
inline constexpr bool always_false = false;

/// convert<T, FromSelector, ToSelector>(From &)
template <class T, typename FromSelector, typename ToSelector>
arg_lookup_t<T, ToSelector> convert(arg_lookup_t<T, FromSelector> &x) {
    if constexpr (std::is_same_v<FromSelector, Input> &&
                  std::is_same_v<ToSelector, Storage>) {
        return arg_traits<T>::input_to_storage(x);
    } else if constexpr (std::is_same_v<FromSelector, Storage> &&
                         std::is_same_v<ToSelector, PassedArgument>) {
        return arg_traits<T>::storage_to_arg(x);
    } else if constexpr (std::is_same_v<FromSelector, Argument> &&
                         std::is_same_v<ToSelector, Compare>) {
        return arg_traits<T>::arg_to_compare(x);
    } else if constexpr (std::is_same_v<FromSelector, Input> &&
                         std::is_same_v<ToSelector, Compare>) {
        return arg_traits<T>::input_to_compare(x);
    } else {
        static_assert(always_false<FromSelector, ToSelector>,
                      "invalid conversion");
    }
}

// convert vectors
template <class T, typename FromSelector, typename ToSelector, class Arg,
          class FromT = arg_lookup_t<T, FromSelector>,
          class ToT = std::remove_const_t<arg_lookup_t<T, ToSelector>>>
    requires std::is_same_v<std::remove_const_t<Arg>,
                            std::vector<std::remove_const_t<FromT>>> ||
             std::is_same_v<std::remove_const_t<Arg>,
                            std::initializer_list<FromT>>
std::vector<ToT> transform_vector(Arg &from) {
    std::vector<ToT> result;
    if constexpr (std::is_const_v<Arg>) {
        std::transform(std::begin(from), std::end(from),
                       std::back_inserter(result),
                       convert<T, FromSelector, ToSelector>);
    } else {
        for (auto &element : from) {
            // can't use std::transform here since it doesn't allow mutating the
            // values pointed to by the input iterator
            // cppcheck-suppress useStlAlgorithm
            result.push_back(convert<T, FromSelector, ToSelector>(element));
        }
    }
    return result;
}

// convert tuples
template <
    typename FromSelector, typename ToSelector, class... Ts, class Arg,
    class From = std::tuple<arg_lookup_t<Ts, FromSelector>...>,
    class FromNoConst =
        std::tuple<std::remove_const_t<arg_lookup_t<Ts, FromSelector>>...>,
    class To = std::tuple<std::remove_const_t<arg_lookup_t<Ts, ToSelector>>...>>
    requires std::is_same_v<std::remove_const_t<Arg>, From> ||
             std::is_same_v<std::remove_const_t<Arg>, FromNoConst>
To transform_tuple(Arg &from) {
    return []<std::size_t... I>(Arg & from, std::index_sequence<I...>) {
        return std::make_tuple(
            convert<Ts, FromSelector, ToSelector>(std::get<I>(from))...);
    }
    (from, std::index_sequence_for<Ts...>{});
}

} // namespace detail

/**********************************************************************
 *                  argument_traits specializations                   *
 **********************************************************************/

// arithmetic types and strings don't need any special handling
template <class T>
    requires std::is_arithmetic_v<T> || std::is_same_v<T, std::string>
struct argument_traits<T> : detail::base_argument_traits<T> {};

// vector
template <class T>
struct argument_traits<std::vector<T>>
    : detail::base_argument_traits<std::vector<T>> {
    using input_type = const std::initializer_list<arg_lookup_t<T, Input>>;
    using storage_type = std::vector<arg_lookup_t<T, Storage>>;
    using compare_type =
        const std::vector<std::remove_const_t<arg_lookup_t<T, Compare>>>;

    static storage_type input_to_storage(const input_type &input) {
        return detail::transform_vector<T, Input, Storage>(input);
    }
    static typename argument_traits::passed_arg_type
    storage_to_arg(storage_type &storage) {
        return detail::transform_vector<T, Storage, PassedArgument>(storage);
    }

    static compare_type
    arg_to_compare(const typename argument_traits::arg_type &arg) {
        return detail::transform_vector<T, Argument, Compare>(arg);
    }
    static compare_type input_to_compare(const input_type &input) {
        return detail::transform_vector<T, Input, Compare>(input);
    }
};

// multiset
template <class T>
struct argument_traits<std::multiset<T>>
    : detail::base_argument_traits<std::multiset<T>> {
    using input_type = const std::initializer_list<T>;
};

// unordered_set
template <class T>
struct argument_traits<std::unordered_set<T>>
    : detail::base_argument_traits<std::unordered_set<T>> {
    using input_type = const std::unordered_set<T>;
};

// tuple
template <class... Ts>
struct argument_traits<std::tuple<Ts...>>
    : detail::base_argument_traits<std::tuple<Ts...>> {
    using input_type = const std::tuple<arg_lookup_t<Ts, Input>...>;
    using storage_type = std::tuple<arg_lookup_t<Ts, Storage>...>;
    using compare_type =
        const std::tuple<std::remove_const_t<arg_lookup_t<Ts, Compare>>...>;

    static storage_type input_to_storage(const input_type &input) {
        return detail::transform_tuple<Input, Storage, Ts...>(input);
    }
    static typename argument_traits::passed_arg_type
    storage_to_arg(storage_type &storage) {
        return detail::transform_tuple<Storage, PassedArgument, Ts...>(storage);
    }

    static compare_type
    arg_to_compare(const typename argument_traits::arg_type &arg) {
        return detail::transform_tuple<Argument, Compare, Ts...>(arg);
    }
    static compare_type input_to_compare(const input_type &input) {
        return detail::transform_tuple<Input, Compare, Ts...>(input);
    }
};
// TODO: I think this should be generalizable to std::pair somehow

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
};

int fix_exit_code(unsigned long count) {
    // max return value is 255, but upper 128 are used for signals
    return static_cast<int>(std::min(count, 127UL));
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
            std::cout << "test case " << test_num << " with "
                      << (result.num_args == 1 ? "input" : "inputs") << " "
                      << result.arguments << " failed:\n"
                      << "       got: " << result.actual << "\n"
                      << "  expected: " << result.expected;
            if (!result.output.empty()) {
                std::cout << "\n  captured output:\n" << result.output;
            }
            std::cout << std::endl;
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
                             arg_lookup_t<R, Input> expected) {
        auto inputs = std::make_tuple(raw_inputs...);
        std::remove_const_t<arg_lookup_t<R, Compare>> result_comp =
            [this, &inputs]() {
                auto storages =
                    detail::convert<args_tuple, Input, Storage>(inputs);
                auto args =
                    detail::convert<args_tuple, Storage, PassedArgument>(
                        storages);

                start_capturing_cout();
                auto result = std::apply(function_under_test, args);
                stop_capturing_cout();
                return detail::convert<R, Argument, Compare>(result);
            }();
        return check_result(result_comp, expected, [&inputs]() {
            return detail::convert<args_tuple, Input, Compare>(inputs);
        });
    }

    TestResult run_on_args(arg_lookup_t<Args, PassedArgument>... raw_args,
                           arg_lookup_t<R, Input> expected) {
        auto args = std::make_tuple(raw_args...);
        start_capturing_cout();
        auto result = std::apply(function_under_test, args);
        stop_capturing_cout();
        auto result_comp = detail::convert<R, Argument, Compare>(result);
        return check_result(result_comp, expected, [&args]() {
            return detail::convert<args_tuple, Argument, Compare>(args);
        });
    }

  private:
    TestResult check_result(const arg_lookup_t<R, Compare> &result_comp,
                            const arg_lookup_t<R, Input> &expected,
                            std::function<arg_lookup_t<args_tuple, Compare>()>
                                get_args_for_printing) {
        auto expected_comp = detail::convert<R, Input, Compare>(expected);
        if (!test_equality(result_comp, expected_comp, float_ulps)) {
            std::ostringstream args_ss, expected_ss, actual_ss, output_ss;
            args_ss << get_args_for_printing();
            // add indents to each line of the output
            for (std::string line; std::getline(captured_cout, line);) {
                output_ss << "  # " << line << '\n';
            }
            pretty_print::write_repr(expected_ss, expected_comp, true);
            pretty_print::write_repr(actual_ss, result_comp, true);
            return {false,           sizeof...(Args),
                    args_ss.str(),   expected_ss.str(),
                    actual_ss.str(), output_ss.str()};
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

    MethodTest(const std::string &filename, C &obj, R (C::*func)(Args...) const,
               int float_ulps = 2)
        : BaseTest(filename, float_ulps), obj(obj),
          runner(_wrap_test_function(this, func), float_ulps) {}

    void operator()(arg_lookup_t<Args, Input>... raw_inputs,
                    arg_lookup_t<R, Input> expected) {
        ++test_num;
        record_result(runner.run_on_inputs(raw_inputs..., expected));
    }

    /// Use this when programmatically generating tests, since
    /// std::initializer_lists can't be constructed at runtime.
    void run_on_args(arg_lookup_t<Args, PassedArgument>... raw_args,
                     arg_lookup_t<R, Input> expected) {
        ++test_num;
        record_result(runner.run_on_args(raw_args..., expected));
    }
};

// explicit template argument deduction guides
template <class C, class R, class... Args>
MethodTest(const std::string &, C &&, R (C::*)(Args...))
    -> MethodTest<C, R, Args...>;
template <class C, class R, class... Args>
MethodTest(const std::string &, C &&, R (C::*)(Args...), int)
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
                    arg_lookup_t<R, Input> expected) {
        ++test_num;
        record_result(runner.run_on_inputs(raw_inputs..., expected));
    }

    /// Use this when programmatically generating tests, since
    /// std::initializer_lists can't be constructed at runtime.
    void run_on_args(arg_lookup_t<Args, PassedArgument>... raw_args,
                     arg_lookup_t<R, Input> expected) {
        ++test_num;
        record_result(runner.run_on_args(raw_args..., expected));
    }
};

template <class R, class... Args>
PureTest(const std::string &, R (*)(Args...)) -> PureTest<R, Args...>;
template <class R, class... Args>
PureTest(const std::string &, R (*)(Args...), int) -> PureTest<R, Args...>;

} // namespace unit_test

#endif /* end of include guard: UNIT_TEST_HPP */
