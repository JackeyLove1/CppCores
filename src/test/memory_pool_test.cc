#include <glog/logging.h>
#include <gtest/gtest.h>
#include "utils/memory_pool_lite.h"
#include "utils/memory_utils.h"

TEST(SimpleAllocatorTest, BasicTest) {
    std::vector<int, SimpleAllocator<int>> v;
    for (int i = 0; i < 1000; ++i) {
        v.push_back(i);
    }
    for (int i = 0; i < 1000; ++i) {
        EXPECT_EQ(v[i], i);
    }
}