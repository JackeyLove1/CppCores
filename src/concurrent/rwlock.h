#pragma once

#include <thread>
#include <atomic>
#include <cassert>
#include <cerrno>
#include <cstring>
#include <pthread.h>
#include <system_error>

#include "lock.h"

class PosixRWLock : public SharedLock {
public:
    enum Kind {
        kKindPreferReader = PTHREAD_RWLOCK_PREFER_READER_NP,

        // Setting the value read-write lock kind to PTHREAD_RWLOCK_PREFER_WRITER_NP,
        // results in the same behavior as setting the value to PTHREAD_RWLOCK_PREFER_READER_NP.
        // As long as a reader thread holds the lock the thread holding a write lock will be
        // starved. Setting the kind value to PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP,
        // allows the writer to run. However, the writer may not be recursive as is implied by the
        // name.
        kKindPreferWriter = PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP,
        kKindDefault = PTHREAD_RWLOCK_DEFAULT_NP,
    };

public:
    explicit PosixRWLock() {
        if (pthread_rwlock_init(&m_rwlock, nullptr)) {
            throw std::system_error(errno, std::generic_category());
        }
    };

    explicit PosixRWLock(Kind kind){
        pthread_rwlockattr_t attr;
        pthread_rwlockattr_init(&attr);
        pthread_rwlockattr_setkind_np(&attr, kind);
        pthread_rwlock_init(&m_rwlock, &attr);
        pthread_rwlockattr_destroy(&attr);
    }

    ~PosixRWLock() override {
        pthread_rwlock_destroy(&m_rwlock);
    };

    void lock() noexcept override {
        pthread_rwlock_wrlock(&m_rwlock);
    }

    void unlock() noexcept override {
        pthread_rwlock_unlock(&m_rwlock);
    }

    bool try_lock() noexcept override {
        int res = pthread_rwlock_trywrlock(&m_rwlock);
        if (res == EBUSY) {
            return false;
        }
        return true;
    }

    void lock_shared() noexcept override {
        pthread_rwlock_rdlock(&m_rwlock);
    }

    void unlock_shared() noexcept override {
        pthread_rwlock_unlock(&m_rwlock);
    }

    bool try_lock_shared() noexcept override {
        return pthread_rwlock_tryrdlock(&m_rwlock);
    }

private:
    pthread_rwlock_t m_rwlock{};
};