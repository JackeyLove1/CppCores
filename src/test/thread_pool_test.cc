#include <gtest/gtest.h>
#include <gflags/gflags.h>
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