#pragma once

#include <memory>
#include <cstdlib>
#include <type_traits>

struct FreeDeleter {
    inline void operator()(void *ptr) const {
        free(ptr);
    }
};

template<typename T>
struct ArrayDeleter {
    inline void operator()(T *array) const {
        delete[] array;
    }
};

template<typename T>
struct ArrayDeleter<T[]> {
    inline void operator()(T *array) const {
        delete[] array;
    }
};


template<typename T, typename D = std::default_delete<T>>
using ScopedPtr = std::unique_ptr<T, D>;

template<typename T>
using ArrayPtr = std::unique_ptr<T, ArrayDeleter<T>>;

template<typename T>
using MallocPtr = std::unique_ptr<T, FreeDeleter>;
