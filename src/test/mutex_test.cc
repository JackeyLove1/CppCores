#include <atomic>
#include <thread>
#include <glog/logging.h>
#include <gtest/gtest.h>
#include "concurrent/mutex.h"
#include "concurrent/scoped_locker.h"

TEST(MutextTest, Basic) {
    MutexBase mutex;
    mutex.lock();
    ASSERT_FALSE(mutex.try_lock());
    mutex.unlock();
    ASSERT_TRUE(mutex.try_lock());
    mutex.unlock();
    {
        std::unique_lock<MutexBase> locker(mutex);
    }
    {
        std::lock_guard<MutexBase> locker(mutex);
    }
}

TEST(MutextTest, MultiThreadTest){
    constexpr int COUNT = 100000;
    int counter = 0;
    MutexBase lock;
    auto threadFunc = [&lock, &counter, COUNT] {
        for (int i = 0; i < COUNT; ++i) {
            std::lock_guard guard(lock);
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

TEST(MutextTest, FastPthreadMutexTest){
    constexpr int COUNT = 100000;
    int counter = 0;
    FastPthreadMutex lock;
    auto threadFunc = [&lock, &counter, COUNT] {
        for (int i = 0; i < COUNT; ++i) {
            std::lock_guard guard(lock);
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