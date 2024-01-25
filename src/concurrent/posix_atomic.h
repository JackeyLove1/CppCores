#pragma once

#include <atomic>

template<typename T>
T AtomicGet(const T *ptr) {
    __sync_synchronize();
    return *ptr;
}

template<typename T>
T AtomicSet(T *ptr, T value) {
    (void) __sync_lock_test_and_set(ptr, value);
    return value;
}

template<typename T>
T AtomicExchange(T *ptr, T value) {
    return __sync_lock_test_and_set(ptr, value);
}

template<typename T>
T AtomicAdd(T *ptr, T value) {
    return __sync_add_and_fetch(ptr, value);
}

template<typename T>
T AtomicSub(T *ptr, T value) {
    return __sync_sub_and_fetch(ptr, value);
}

template<typename T>
T AtomicOr(T *ptr, T value) {
    return __sync_or_and_fetch(ptr, value);
}

template<typename T>
T AtomicAnd(T *ptr, T value) {
    return __sync_and_and_fetch(ptr, value);
}

template<typename T>
T AtomicXor(T *ptr, T value) {
    return __sync_xor_and_fetch(ptr, value);
}

template<typename T>
T AtomicIncrement(T *ptr) {
    return __sync_add_and_fetch(ptr, 1);
}

template<typename T>
T AtomicDecrement(T *ptr) {
    return __sync_sub_and_fetch(ptr, 1);
}

template<typename T>
T AtomicExchangeAdd(T *ptr, T value) {
    return __sync_fetch_and_add(ptr, value);
}

template<typename T>
T AtomicExchangeSub(T *ptr, T value) {
    return __sync_fetch_and_sub(ptr, value);
}

template<typename T>
T AtomicExchangeOr(T *ptr, T value) {
    return __sync_fetch_and_or(ptr, value);
}

template<typename T>
T AtomicExchangeAnd(T *ptr, T value) {
    return __sync_fetch_and_and(ptr, value);
}

template<typename T>
T AtomicExchangeXor(T *ptr, T value) {
    return __sync_fetch_and_xor(ptr, value);
}

template<typename T>
bool AtomicCompareExchange(T *ptr, T compare, T exchange, T *old) {
    *old = *ptr;
    if (__sync_bool_compare_and_swap(ptr, compare, exchange)) {
        *old = compare;
        return true;
    }
    return false;
}

template<typename T>
bool AtomicCompareExchange(T *ptr, T compare, T exchange) {
    return __sync_bool_compare_and_swap(ptr, compare, exchange);
}


template<typename T>
class AtomicPointer {
private:
    std::atomic<T *> holder_;

public:
    AtomicPointer() : holder_{nullptr} {}

    explicit AtomicPointer(T *ptr) : holder_{ptr} {}

    inline T *Load() const {
        return holder_.load(std::memory_order_acquire);
    }

    inline void Store(T *ptr) {
        holder_.store(std::memory_order_release);
    }

    inline T *Load_Relaxed() const {
        return holder_.load(std::memory_order_relaxed);
    }

    inline void Store_Relaxed(void *v) {
        holder_.store(v, std::memory_order_relaxed);
    }
};