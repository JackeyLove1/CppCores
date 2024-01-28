#include <iostream>
#include <thread>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include <boost/lockfree/queue.hpp>
#include "concurrent/lockfree_queue.h"

TEST(LockFreeQueueTest, SpscBasic) {
    const size_t N = 100;
    SpscLockFreeQueue<int, N> q;
    for (auto i = 0u; i < N - 1; ++i) {
        ASSERT_TRUE(q.push(i)) << i;
    }
    for (auto i = 0u; i < N - 1; ++i) {
        int x;
        ASSERT_TRUE(q.pop(x)) << i;
        ASSERT_EQ(x, i) << i;
    }

    for (auto i = 0u; i < N - 1; ++i) {
        ASSERT_TRUE(q.push(i)) << i;
    }
    for (auto i = 0u; i < N - 1; ++i) {
        auto opt = q.pop();
        ASSERT_TRUE(opt.has_value());
        ASSERT_EQ(opt.value(), i);
    }
}

TEST(LockFreeQueueTest, DISABLED_MpmcBasic) {
    const size_t N = 100;
    MpmcLockFreeQueue<int, N> q;
    for (auto i = 0u; i < N - 1; ++i) {
        ASSERT_TRUE(q.push(i)) << i;
    }
    for (auto i = 0u; i < N - 1; ++i) {
        int x;
        ASSERT_TRUE(q.pop(x)) << i;
        ASSERT_EQ(x, i) << i;
    }

    for (auto i = 0u; i < N - 1; ++i) {
        ASSERT_TRUE(q.push(i)) << i;
    }
    for (auto i = 0u; i < N - 1; ++i) {
        auto opt = q.pop();
        ASSERT_TRUE(opt.has_value());
        ASSERT_EQ(opt.value(), i);
    }
}

TEST(BoostLockFreeQueueTest, SingleThreadTest) {
    constexpr size_t N = 128, LOOP = 10000;
    boost::lockfree::queue<int> queue(N);
    auto producer = [&queue]() {
        for (auto i = 0u; i < LOOP; ++i) {
            while (!queue.push(i));
        }
    };

    auto consumer = [&queue]() {
        int value;
        while (queue.pop(value));
    };

    std::thread p1(producer);
    std::thread c1(consumer);
    p1.join();
    c1.join();
}

TEST(BoostLockFreeQueueTest, MultiThreadTest) {
    constexpr size_t LOOP = 10000, N = LOOP * 3;
    boost::lockfree::queue<size_t> queue(N);
    std::atomic<size_t> count{0};
    auto producer = [&queue]() {
        for (auto i = 0u; i < LOOP; ++i) {
            while (!queue.push(1));
        }
    };

    auto consumer = [&]() {
        int value;
        while (queue.pop(value)) {
            count.fetch_add(value, std::memory_order_relaxed);
        }
    };

    std::thread p1(producer), p2(producer);
    std::thread c1(consumer), c2(consumer), c3(consumer);
    p1.join();
    p2.join();
    c1.join();
    c2.join();
    c3.join();
    ASSERT_EQ(count.load(), LOOP * 2);
}

TEST(BoostLockFreeQueueTest, BoundQueu) {
    constexpr size_t LOOP = 10000, N = 128;
    boost::lockfree::queue<size_t> queue(N);
    std::atomic<size_t> count{0};
    std::atomic<bool> producersDone{false};
    auto producer = [&queue]() {
        for (auto i = 0u; i < LOOP; ++i) {
            while (!queue.push(1)){
                std::this_thread::yield();
            }
        }
    };

    auto consumer = [&]() {
        int value;
        while (!(producersDone.load() && queue.empty())){
            while (queue.pop(value)) {
                count.fetch_add(value, std::memory_order_relaxed);
            }
        }
    };

    std::thread p1(producer), p2(producer);
    std::thread c1(consumer);
    p1.join();
    p2.join();
    producersDone.store(true);
    c1.join();
    ASSERT_EQ(count.load(), LOOP * 2);
}