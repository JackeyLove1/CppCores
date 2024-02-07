#pragma once

#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cxxabi.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <cstdio>
#include <cstdint>
#include <vector>
#include <string>
#include <iomanip>
#include <execinfo.h>

#include <glog/logging.h>

static std::string fiber_demangle(const char *str) {
    size_t size = 0;
    int status = 0;
    std::string rt;
    rt.resize(256);
    if (1 == sscanf(str, "%*[^(]%*[^_]%255[^)+]", &rt[0])) {
        char *v = abi::__cxa_demangle(&rt[0], nullptr, &size, &status);
        if (v) {
            std::string result(v);
            free(v);
            return result;
        }
    }
    if (1 == sscanf(str, "%255s", &rt[0])) {
        return rt;
    }
    return str;
}

void FiberBacktrace(std::vector<std::string> &bt, int size, int skip) {
    void **array = (void **) malloc((sizeof(void *) * size));
    size_t s = ::backtrace(array, size);

    char **strings = backtrace_symbols(array, s);
    if (strings == nullptr) {
        LOG(ERROR) << "backtrace_symbols error"; // free(array)
        return;
    }

    for (size_t i = skip; i < s; ++i) {
        bt.push_back(fiber_demangle(strings[i]));
    }

    free(strings);
    free(array);
}

std::string FiberBacktraceToString(int size = 64, int skip = 2, const std::string &prefix = "") {
    std::vector<std::string> bt;
    FiberBacktrace(bt, size, skip);
    std::stringstream ss;
    for (size_t i = 0; i < bt.size(); ++i) {
        ss << prefix << bt[i] << std::endl;
    }
    return ss.str();
}

/// most likely true
#define FIBER_LIKELY(x)       __builtin_expect(!!(x), 1)
/// most likely false
#define FIBER_UNLIKELY(x)       __builtin_expect(!!(x), 0)

/// Assertion
#define FIBER_ASSERT(x) \
    if(FIBER_UNLIKELY(!(x))) { \
        LOG(ERROR) << "ASSERTION: " #x \
            << "\nbacktrace:\n" \
            << FiberBacktraceToString(100, 2, "    "); \
        assert(x); \
    }

#define FIBER_ASSERT_MSG(x, w) \
    if(FIBER_UNLIKELY(!(x))) { \
        LOG(ERROR) << "ASSERTION: " #x \
            << "\n" << w \
            << "\nbacktrace:\n" \
            << FiberBacktraceToString(100, 2, "    "); \
        assert(x); \
    }