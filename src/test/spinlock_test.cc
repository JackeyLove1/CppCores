#include <atomic>
#include <thread>
#include <glog/logging.h>
#include <gtest/gtest.h>
#include "concurrent/spinlock.h"

TEST(SpinLockTest, SingleThread) {
    SimpleSpinLock simple_spinlock;
    ASSERT_NO_THROW(simple_spinlock.lock());
    ASSERT_NO_THROW(simple_spinlock.unlock());

    AtomicSpinLock atomic_spinlock;
    ASSERT_NO_THROW(atomic_spinlock.lock());
    ASSERT_NO_THROW(atomic_spinlock.unlock());

    PosixSpinLock posix_spinlock;
    ASSERT_NO_THROW(posix_spinlock.lock());
    ASSERT_NO_THROW(posix_spinlock.unlock());
}

TEST(SpinLockTest, TryLock) {
    SimpleSpinLock simple_spinlock;
    ASSERT_TRUE(simple_spinlock.try_lock());
    ASSERT_FALSE(simple_spinlock.try_lock());
    simple_spinlock.unlock();
    ASSERT_TRUE(simple_spinlock.try_lock());
    simple_spinlock.unlock();

    AtomicSpinLock atomic_spinlock;
    ASSERT_TRUE(atomic_spinlock.try_lock());
    ASSERT_FALSE(atomic_spinlock.try_lock());
    atomic_spinlock.unlock();
    ASSERT_TRUE(atomic_spinlock.try_lock());
    atomic_spinlock.unlock();
}

void STDLockSpinLock(Lock &lock) {
    {
        std::unique_lock guard(lock);
    }
    {
        std::lock_guard guard(lock);
    }
}

TEST(SpinLockTest, STDLockTest) {
    SimpleSpinLock simple_spinlock;
    STDLockSpinLock(simple_spinlock);

    AtomicSpinLock atomic_spinlock;
    STDLockSpinLock(atomic_spinlock);

    PosixSpinLock posix_spinlock;
    STDLockSpinLock(posix_spinlock);
}

void SpinLockMultiThreadTest(Lock &lock) {
    constexpr int COUNT = 100000;
    int counter = 0;

    auto threadFunc = [&lock, &counter, COUNT] {
        for (int i = 0; i < COUNT; ++i) {
            std::unique_lock guard(lock);
            counter++;
        }
    };

    std::vector<std::thread> threads;
    for (size_t i = 0; i < 10; ++i) {
        threads.emplace_back(threadFunc);
    }
    for (auto &&thread: threads) {
        thread.join();
    }

    ASSERT_EQ(counter, COUNT * 10);
}

TEST(SpinLockTest, MultipleThreads) {
    SimpleSpinLock simple_spinlock;
    SpinLockMultiThreadTest(simple_spinlock);

    AtomicSpinLock atomic_spinlock;
    SpinLockMultiThreadTest(atomic_spinlock);

    PosixSpinLock posix_spinlock;
    SpinLockMultiThreadTest(posix_spinlock);
}