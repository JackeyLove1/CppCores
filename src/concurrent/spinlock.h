#pragma once

#include <atomic>
#include <functional>
#include <memory>

#include "lock.h"
#include "this_thread.h"

class SimpleSpinLock : public Lock {
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
public:
    void lock() noexcept override {
        while (flag.test_and_set(std::memory_order_acquire)) {
            // wait/spin until the flag is cleared (unlock is called)
        }
    }

    bool try_lock() noexcept override {
        return !flag.test_and_set(std::memory_order_acquire);
    }

    void unlock() noexcept override {
        flag.clear(std::memory_order_release);
    }
};

class AtomicSpinLock : public Lock {
private:
    std::atomic<bool> lockState{false};

public:
    void lock() noexcept override {
        while (true) {
            // If lock is not taken, stop spinning
            if (!lockState.exchange(true, std::memory_order_acquire)) {
                return;
            }
            // Yield to other threads when lock is not available
            while (lockState.load(std::memory_order_relaxed)) {
                // Pause instruction to prevent excess processor bus usage
                __builtin_ia32_pause();
            }
        }
    }

    bool try_lock() noexcept override {
        // Try to acquire lock without spinning
        return !lockState.exchange(true, std::memory_order_acquire);
    }

    void unlock() noexcept override {
        lockState.store(false, std::memory_order_release);
    }
};

class PosixSpinLock : public Lock {
private:
    pthread_spinlock_t m_lock{};
    pid_t m_owner;

public:
    PosixSpinLock() {
        pthread_spin_init(&m_lock, 0);
        m_owner = 0;
    }

    ~PosixSpinLock() override {
        pthread_spin_destroy(&m_lock);
        m_owner = -1;
    }

    void lock() noexcept override {
        pthread_spin_lock(&m_lock);
        m_owner = ThisThread::GetId();
    }

    bool try_lock() noexcept override {
        if (pthread_spin_trylock(&m_lock)) {
            m_owner = ThisThread::GetId();
            return true;
        }
        return false;
    }

    void unlock() noexcept override {
        m_owner = 0;
        pthread_spin_unlock(&m_lock);
    }
};