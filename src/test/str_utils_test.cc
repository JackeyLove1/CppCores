#include <algorithm>
#include <array>
#include <gtest/gtest.h>
#include "utils/utils.h"
#include "utils/str_utils.h"

class StrUtilsTest : public ::testing::Test {
public:
    void SetUp() override {}

    void TearDown() override {}
};

TEST_F(StrUtilsTest, LowerUpperTest) {
    std::string upper = "ABCDEFGHIJKLMN";
    std::string lower = "abcdefghijklmn";

    EXPECT_EQ(toLower(std::cref(upper)), lower);
    EXPECT_EQ(toUpper(std::cref(lower)), upper);
}

TEST_F(StrUtilsTest, PrefixAndSuffix) {
    const std::string str = "helloworld";
    const std::string prefix = "hello";
    const std::string suffix = "world";

    EXPECT_TRUE(StartWithPrefix(str, prefix));
    EXPECT_FALSE(StartWithPrefix(str, suffix));

    EXPECT_TRUE(EndWithSuffix(str, suffix));
    EXPECT_FALSE(EndWithSuffix(str, prefix));

    auto sub1 = RemovePrefix(str, prefix);
    EXPECT_EQ(sub1, suffix);
    auto sub2 = RemoveSuffix(str, suffix);
    EXPECT_EQ(sub2, prefix);
}