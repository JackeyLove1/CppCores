#include <algorithm>
#include <array>
#include <random>
#include <gtest/gtest.h>
#include "utils/utils.h"

class UtilsTest: public ::testing::Test{
public:
    void SetUp() override {}

    void TearDown() override {}
public:
    static constexpr size_t kSize = 100;
};

TEST_F(UtilsTest, RandomNumberTest){
    std::vector<int> container(kSize, 0);
    for (size_t i =  0; i < kSize; ++i){
        container[i] = GenRandom<int>();
    }
    std::sort(container.begin(), container.end());
    container.erase(std::unique(container.begin(),container.end()), container.end());
    ASSERT_EQ(container.size(), kSize);
}