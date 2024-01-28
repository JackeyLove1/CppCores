#include <cstring>
#include <algorithm>
#include <sstream>
#include <glog/logging.h>
#include "stack_trace.h"

template<typename T, size_t N>
char (&ArraySizeHelper(const T (&array)[N]))[N];

#define arraysize(array) (sizeof(ArraySizeHelper(array)))
#define ARRAY_SIZE(array) arraysize(array)

StackTrace::StackTrace(const void *const *trace, size_t count) {
    count = std::min(count, arraysize(trace_));
    if (count)
        std::memcpy(trace_, trace, count * sizeof(trace_[0]));
    count_ = count;
}

StackTrace::~StackTrace() {
}

const void *const *StackTrace::Addresses(size_t *count) const {
    *count = count_;
    if (count_)
        return trace_;
    return nullptr;
}

std::string StackTrace::ToString() const {
    std::stringstream stream;
    // OutputToStream(&stream);
    return stream.str();
}

// NOTE: code from sandbox/linux/seccomp-bpf/demo.cc.
char *itoa_r(intptr_t i, char *buf, size_t sz, int base, size_t padding) {
    // Make sure we can write at least one NUL byte.
    size_t n = 1;
    if (n > sz)
        return NULL;

    if (base < 2 || base > 16) {
        buf[0] = '\000';
        return NULL;
    }

    char *start = buf;

    uintptr_t j = i;

    // Handle negative numbers (only for base 10).
    if (i < 0 && base == 10) {
        j = -i;

        // Make sure we can write the '-' character.
        if (++n > sz) {
            buf[0] = '\000';
            return NULL;
        }
        *start++ = '-';
    }

    // Loop until we have converted the entire number. Output at least one
    // character (i.e. '0').
    char *ptr = start;
    do {
        // Make sure there is still enough space left in our output buffer.
        if (++n > sz) {
            buf[0] = '\000';
            return NULL;
        }

        // Output the next digit.
        *ptr++ = "0123456789abcdef"[j % base];
        j /= base;

        if (padding > 0)
            padding--;
    } while (j > 0 || padding > 0);

    // Terminate the output with a NUL character.
    *ptr = '\000';

    // Conversion to ASCII actually resulted in the digits being in reverse
    // order. We can't easily generate them in forward order, as we can't tell
    // the number of characters needed until we are done converting.
    // So, now, we reverse the string (except for the possible "-" sign).
    while (--ptr > start) {
        char ch = *ptr;
        *ptr = *start;
        *start++ = ch;
    }
    return buf;
}

void print_stacktrace() {
    // Length of the stack trace
    const int max_stack_trace_size = 100;

    void *array[max_stack_trace_size];
    size_t stack_num = backtrace(array, max_stack_trace_size);

    // Obtain symbols of each stack frame
    char **stacktrace_symbols = backtrace_symbols(array, stack_num);
    if (stacktrace_symbols == nullptr) {
        LOG(ERROR) << ("backtrace_symbols");
        exit(EXIT_FAILURE);
    }

    // Prints each stack trace frame
    for (size_t i = 0; i < stack_num; i++) {
        LOG(ERROR) << stacktrace_symbols[i];
    }

    //cleanup
    free(stacktrace_symbols);
}