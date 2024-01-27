#include <gtest/gtest.h>
#include <gflags/gflags.h>
#include "utils/utils.h"
#include "concurrent/thread_pool.h"

TEST(ThreadPoolTest, SimpleThreadPoolTest) {
    const size_t nums = 16, tasks = 10000;
    SimpleThreadPool pool(nums);
    std::atomic<size_t> counter{0};

    for (size_t i = 0; i < tasks; ++i) {
        pool.enqueue([&]() {
            counter.fetch_add(1, std::memory_order_release);
        });
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    ASSERT_GE(counter, tasks / 2); // Assert that at least 1 task has been executed

    // Sleep more to make sure all tasks will be completed
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    ASSERT_EQ(counter, tasks);
}

TEST(ThreadPoolTest, ToftThreadPoolPrintTest1) {
    /// workers == tasks
    ToftThreadPool pool(4);
    constexpr int loop1 = 4;
    for (int i = 0; i < loop1; ++i) {
        pool.AddTask([]() { LOG(INFO) << "Run ..."; });
    }
}

TEST(ThreadPoolTest, ToftThreadPoolPrintTest2) {
    /// workers > tasks
    ToftThreadPool pool(4);
    constexpr int loop1 = 2;
    for (int i = 0; i < loop1; ++i) {
        pool.AddTask([]() { LOG(INFO) << "Run ..."; });
    }
}


TEST(ThreadPoolTest, ToftThreadPoolPrintTest3) {
    /// workers < tasks
    ToftThreadPool pool(2);
    constexpr int loop1 = 4;
    for (int i = 0; i < loop1; ++i) {
        pool.AddTask([]() { LOG(INFO) << "Run ..."; });
    }
}

TEST(ThreadPoolTest, DISABLED_ToftThreadPoolTest) {
    ToftThreadPool pool(16);
    std::atomic<int> counter{0};
    constexpr int loop1 = 2, loop2 = 2;
    for (int i = 0; i < loop1; ++i) {
        for (int j = 0; j < loop2; ++j) {
            pool.AddTask(std::bind([&](std::atomic<int>&cnt){cnt.fetch_add(1);}, std::ref(counter)));
        }
    }
    ASSERT_EQ(counter.load(), loop1 * loop2);
}

TEST(ThreadTask, ToftThreadPoolDestory) {

}

TEST(ThreadTask, ToftThreadPoolPerformence) {

}