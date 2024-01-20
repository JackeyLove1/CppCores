#include <algorithm>
#include <array>
#include <random>
#include <gtest/gtest.h>
#include "utils/utils.h"
#include "utils/defer.h"

class DeferTest : public ::testing::Test {
public:
    void SetUp() override {}

    void TearDown() override {}

public:

};

TEST_F(DeferTest, BasicTest) {
    auto inc = GenRandom<int>();
    int a = 1, b = 2, c = 3;
    {
        DEFER([&]() { a = a + inc; });
        DEFER([&]() { b = b + inc; });
        DEFER([&]() { c = c + inc; });
    }
    ASSERT_EQ(1, a - inc);
    ASSERT_EQ(2, b - inc);
    ASSERT_EQ(3, c - inc);
}