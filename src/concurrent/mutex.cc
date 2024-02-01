#include <atomic>
#include <pthread.h>
#include "mutex.h"
#include "atomicops.h"
#include "sys_futex.h"

// Implement bthread_mutex_t related functions
struct MutexInternal {
    std::atomic<unsigned char> locked;
    std::atomic<unsigned char> contended;
    unsigned short padding;
};

static constexpr MutexInternal MUTEX_CONTENDED_RAW = {{1}, {1}, 0};
static constexpr MutexInternal MUTEX_LOCKED_RAW = {{1}, {0}, 0};

// Define as macros rather than constants which can't be put in read-only
// section and affected by initialization-order fiasco.
#define BTHREAD_MUTEX_CONTENDED (*(const unsigned*)&MUTEX_CONTENDED_RAW) // 257
#define BTHREAD_MUTEX_LOCKED (*(const unsigned*)&MUTEX_LOCKED_RAW) // 1

static_assert(sizeof(unsigned) == sizeof(MutexInternal), "sizeof_mutex_internal_must_equal_unsigned");

int FastPthreadMutex::lock_contended() {
    std::atomic<unsigned> *whole = reinterpret_cast<std::atomic<unsigned> *>(&_futex);
    while (whole->exchange(BTHREAD_MUTEX_CONTENDED) & BTHREAD_MUTEX_LOCKED) { // whole为0加锁成功
        if (futex_wait_private(whole, BTHREAD_MUTEX_CONTENDED, nullptr) < 0
            && errno != EWOULDBLOCK) {
            return errno;
        }
    }
    return 0;
}

void FastPthreadMutex::lock() noexcept {
    MutexInternal *split = reinterpret_cast<MutexInternal *>(&_futex);
    if (split->locked.exchange(1, std::memory_order_acquire)) {
        (void) lock_contended();
    }
}

bool FastPthreadMutex::try_lock() noexcept {
    MutexInternal *split = reinterpret_cast<MutexInternal *>(&_futex);
    return !split->locked.exchange(1, std::memory_order_acquire);
}

void FastPthreadMutex::unlock() noexcept {
    std::atomic<unsigned> *whole = reinterpret_cast<std::atomic<unsigned> *>(&_futex);
    const unsigned prev = whole->exchange(0, std::memory_order_release);
    // CAUTION: the mutex may be destroyed, check comments before butex_create
    if (prev != BTHREAD_MUTEX_LOCKED) {
        futex_wake_private(whole, 1);
    }
}