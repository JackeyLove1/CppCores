#include <cassert>
#include <cerrno>
#include <pthread.h>
#include <cstring>
#include <stdexcept>
#include <string>

#include "lock.h"
#include "this_thread.h"

class MutexBase : public Lock {
public:
    MutexBase() {
        pthread_mutex_init(&m_mutex, nullptr);
    }

    ~MutexBase() {
        pthread_mutex_destroy(&m_mutex);
    }

    void lock() noexcept override {
        pthread_mutex_lock(&m_mutex);
    }

    void unlock() noexcept override {
        pthread_mutex_unlock(&m_mutex);
    }

    bool try_lock() noexcept override {
        return !pthread_mutex_trylock(&m_mutex);
    }

private:
    pthread_mutex_t m_mutex;
};

class NullLock : public Lock {
private:
    bool m_locked{false};
    pid_t m_pid{};
public:
    NullLock() = default;

    void lock() noexcept override {
        m_locked = true;
        m_pid = ThisThread::GetId();
    }

    void unlock() noexcept override {
        m_locked = false;
    }

    bool try_lock() noexcept override {
        m_pid = ThisThread::GetId();
        return m_locked = true;
    }

};