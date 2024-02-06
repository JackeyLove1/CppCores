#include <memory>
#include <atomic>
#include <gtest/gtest.h>
#include <glog/logging.h>

#include "fiber/fiber_thread.h"

class FiberThreadTest : public ::testing::Test {
public:
    void SetUp() override {}

    void TearDown() override {}

public:

};

TEST_F(FiberThreadTest, BasicTest) {
    constexpr int thread_nums = 8, loop_nums = 100000;
    std::atomic<int> counter{0};
    std::vector<FiberThread::ptr> threads;
    for (int i = 0; i < thread_nums; ++i) {
        FiberThread::ptr thread = std::make_shared<FiberThread>([&counter, loop_nums]() {
            for (int i = 0; i < loop_nums; ++i) {
                counter.fetch_add(1, std::memory_order_relaxed);
            }
        }, "thread_" + std::to_string(i));
        threads.push_back(std::move(thread));
    }
    for (auto &&thread: threads) {
        thread->join();
    }
    ASSERT_EQ(counter.load(), loop_nums * thread_nums);
}