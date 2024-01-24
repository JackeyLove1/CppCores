#include <algorithm>
#include <array>
#include <random>
#include <gtest/gtest.h>
#include "utils/utils.h"
#include "utils/uint128_t.h"

class UtilsTest : public ::testing::Test {
public:
    void SetUp() override {}

    void TearDown() override {}

public:
    static constexpr size_t kSize = 100;
};

TEST_F(UtilsTest, RandomNumberTest) {
    std::vector<int> container(kSize, 0);
    for (size_t i = 0; i < kSize; ++i) {
        container[i] = GenRandom<int>();
    }
    std::sort(container.begin(), container.end());
    container.erase(std::unique(container.begin(), container.end()), container.end());
    ASSERT_EQ(container.size(), kSize);
}

TEST_F(UtilsTest, UUIDTest) {
    std::vector<std::string> container;
    container.reserve(kSize);
    for (size_t i = 0; i < kSize; ++i) {
        container.emplace_back(GenerateUUID());
    }
    std::sort(container.begin(), container.end());
    container.erase(std::unique(container.begin(), container.end()), container.end());
    ASSERT_EQ(container.size(), kSize);
}

TEST_F(UtilsTest, UINT128) {
    uint32_t value = 123456789;
    uint128 t1{value};
    EXPECT_EQ(static_cast<uint32_t>(t1), value);

}