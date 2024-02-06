#pragma once

#include <memory>
#include <type_traits>
#include <thread>

template<class T, class X, size_t N>
T &GetInstanceX() {
    static_assert(std::is_default_constructible_v<T>, "T must be default constructible");
    static T v;
    return v;
}

template<class T, class X, size_t N>
std::shared_ptr<T> GetInstancePtr() {
    static_assert(std::is_default_constructible_v<T>, "T must be default constructible");
    static std::shared_ptr<T> v(new T);
    return v;
}

template<class T, class X = void, size_t N = 0>
class FiberSingleton {
    static_assert(std::is_default_constructible_v<T>, "T must be default constructible");
public:
    static T *GetInstance() {
        static T v;
        return &v;
    }
};

template<class T, class X = void, size_t N = 0>
class FiberSingletonPtr {
    static_assert(std::is_default_constructible_v<T>, "T must be default constructible");
public:
    static std::shared_ptr<T> GetInstance() {
        static std::shared_ptr<T> v(new T);
        return v;
    }
};