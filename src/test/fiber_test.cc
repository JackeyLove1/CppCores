#include <algorithm>
#include <random>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include "utils/utils.h"
#include "fiber/fibers.h"
#include "fiber/fiber_thread.h"

void run_in_fiber() {
    LOG(INFO) << "Fiber beginning .... ";
    Fiber::YieldToHold();
    LOG(INFO) << "Fiber ending ... ";
    Fiber::YieldToHold();
}

void test_fiber() {
    LOG(INFO) << "Main begin -1";
    {
        Fiber::GetThis();
        LOG(INFO) << "Main begin";
        Fiber::ptr fiber(new Fiber(run_in_fiber));
        fiber->swapIn();
        LOG(INFO) << "Main after swap in";
        fiber->swapIn();
        LOG(INFO) << "Main after end";
        fiber->swapIn();
    }
    LOG(INFO) << "Main end 2";
}

TEST(FiberTest, DISABLED_Test1) {
    FiberThread::SetName("main");
    std::vector<FiberThread::ptr> threads;
    for (int i = 0; i < 1; ++i) {
        threads.emplace_back(new FiberThread(&test_fiber, "thread_" + std::to_string(i)));
    }
    for (auto &&thread: threads) {
        thread->join();
    }
}