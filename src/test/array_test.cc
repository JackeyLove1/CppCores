#include <algorithm>
#include <random>
#include <gtest/gtest.h>
#include "utils/utils.h"
#include "array.h"


class ArrayTest : public ::testing::Test {
public:
    void SetUp() {}

    void TearDown() {}

public:
    std::mt19937 gen{GetRandomEngine()};

    static constexpr int N = 1000;

};

TEST_F(ArrayTest, BasicTest) {
    Array<int> arr{N};
    ASSERT_EQ(static_cast<std::decay_t<decltype(N)>>(arr.size()), N);
    for (int i = 0; i < N; ++i) {
        arr[i] = i;
    }
    for (int i = 0; i < N; ++i) {
        EXPECT_EQ(i, arr[i]);
    }
    EXPECT_TRUE(arr.is_sorted());

    std::shuffle(arr.begin(), arr.end(), gen);
    EXPECT_FALSE(arr.is_sorted());
    arr.sort();
    EXPECT_TRUE(arr.is_sorted());
}
