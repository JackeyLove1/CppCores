#include <atomic>
#include <mutex>
#include <thread>
#include <glog/logging.h>
#include <gtest/gtest.h>
#include "concurrent/spinlock.h"
#include "concurrent/scoped_locker.h"

TEST(ScopedLocker, BaseTest1) {
    std::mutex mutex;
    {
        CommonScopedLocker locker(mutex);
    }
}

TEST(ScopedLocker, MultiThreadTest1) {
    constexpr int COUNT = 1000;
    int counter = 0;
    std::mutex mutex;
    auto threadFunc = [&mutex, &counter, COUNT] {
        for (int i = 0; i < COUNT; ++i) {
            CommonScopedLocker guard(mutex);
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


TEST(ScopedLocker, BaseTest2) {
    SimpleSpinLock spinlock;
    {
        BaseScopedLocker locker(spinlock);
    }
}

TEST(ScopedLocker, MultiThreadTest2) {
    constexpr int COUNT = 1000;
    int counter = 0;
    SimpleSpinLock lock;
    auto threadFunc = [&lock, &counter, COUNT] {
        for (int i = 0; i < COUNT; ++i) {
            BaseScopedLocker guard(lock);
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