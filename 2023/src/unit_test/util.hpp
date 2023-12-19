#ifndef UTIL_HPP
#define UTIL_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <typeinfo>

template <typename... T>
struct Debug;
#define reveal_type(x)                                                         \
    Debug<decltype(x)> {}

namespace util {
// demangle() adapted from https://stackoverflow.com/a/4541470

std::string demangle(const char *name);

std::string demangle(const std::string &name) { return demangle(name.c_str()); }
std::string demangle(const std::type_info &ti) { return demangle(ti.name()); }

template <class T>
std::string type(const T &t) {
    return demangle(typeid(t).name());
}

/**
 * Guard class that redirects an ostream to an arbitary streambuf, and restores
 * it when destroyed.
 *
 * From https://stackoverflow.com/a/5419388
 */
struct StreamRedirector {
    explicit StreamRedirector(std::streambuf *buf,
                              std::ostream &stream = std::cout)
        : captured(stream), old(captured.rdbuf(buf)) {}
    ~StreamRedirector() { captured.rdbuf(old); }

  private:
    std::ostream &captured;
    std::streambuf *old;
};

struct CaptureStream {
    explicit CaptureStream(std::ostream &stream = std::cout)
        : ss(), redir(ss.rdbuf(), stream) {}
    ~CaptureStream() {}
    std::string str() const { return ss.str(); }

  private:
    std::ostringstream ss;
    StreamRedirector redir;
};

} // namespace util

#ifdef __GNUG__
#include <cstdlib>
#include <cxxabi.h>
#include <memory>

std::string util::demangle(const char *name) {
    int status = -4; // some arbitrary value to eliminate the compiler warning

    // enable c++11 by passing the flag -std=c++11 to g++
    std::unique_ptr<char, void (*)(void *)> res{
        abi::__cxa_demangle(name, nullptr, nullptr, &status), std::free};

    return (status == 0) ? res.get() : name;
}

#else

// does nothing if not g++
std::string util::demangle(const char *name) { return name; }

#endif

#endif /* end of include guard: UTIL_HPP */
