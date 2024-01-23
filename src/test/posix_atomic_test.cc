#include <thread>
#include <atomic>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include "concurrent/posix_atomic.h"
#include "utils/utils.h"

namespace AtomicTest {

}

TEST(Atomic, DISABLED_Value) {
    // TODO(Jacky): FIX ME!
    std::atomic<uint32_t> atomic_counter{0};
    int posix_atomic_counter;
    constexpr size_t num_threads = 8;
    constexpr size_t num_in_loop = 10000;
    std::vector<std::thread> threads{};
    for (size_t i = 0; i < num_threads; ++i) {
        threads.emplace_back([&]() {
            for (size_t j = 0; j < num_in_loop; ++j) {
                atomic_counter.fetch_add(1, std::memory_order_relaxed);
                AtomicAdd(&posix_atomic_counter, 1);
            }
        });
    }
    for (auto &&thread: threads) {
        thread.join();
    }
    EXPECT_EQ(atomic_counter.load(), static_cast<uint32_t>(num_threads * num_in_loop));
    EXPECT_EQ(posix_atomic_counter, static_cast<uint32_t>(num_threads * num_in_loop));
}