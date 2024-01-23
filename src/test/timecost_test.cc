#include <algorithm>
#include <random>
#include <chrono>
#include <ctime>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include "utils/time.h"


class TimeCostTest : public ::testing::Test {
public:
    void SetUp() override {}

    void TearDown() override {}
};

TEST_F(TimeCostTest, BasicTest) {
    TimeCost cost{};
    sleep(1);
    EXPECT_EQ(cost.ElapsedSec(), static_cast<int64_t>(1));
    cost.Reset();
    SleepThread(1);
    EXPECT_EQ(cost.ElapsedSec(), static_cast<int64_t>(1));
}


