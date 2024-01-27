#pragma once

#include <type_traits>
#include <functional>
#include <mutex>

#include "lock.h"

template<typename T, typename = std::void_t<T>>
struct has_lock_method : std::false_type {
};

template<typename T>
struct has_lock_method<T, std::void_t<decltype(std::declval<T>().lock())>> : public std::true_type {
};

template<typename T, typename = std::void_t<T>>
struct has_unlock_method : std::false_type {
};

template<typename T>
struct has_unlock_method<T, std::void_t<decltype(std::declval<T>().unlock())>> : public std::true_type {
};


template<typename LockType,
        typename std::enable_if_t<has_lock_method<Lock>::value, bool> = true,
        typename std::enable_if_t<has_unlock_method<LockType>::value, bool> = true
>
class CommonScopedLocker {
public:
    LockType *m_lock;

public:
    explicit CommonScopedLocker(LockType &lock) : m_lock(std::addressof(lock)) {
        m_lock->lock();
    }

    ~CommonScopedLocker() {
        m_lock->unlock();
    }
};

/*
 * @brief used for lock.h
 */
template<typename LockType,
        typename std::enable_if_t<std::is_base_of_v<Lock, LockType>, void *> = nullptr>
class BaseScopedLocker : public CommonScopedLocker<LockType> {

public:
    explicit BaseScopedLocker(LockType &lock) : CommonScopedLocker<LockType>(lock) {}
};
