#include <optional>
#include <bits/stdc++.h>
#include <gtest/gtest.h>
#include <glog/logging.h>

#include "utils/utils.h"

std::optional<std::string> create1(bool flag) {
    if (flag) {
        return GenerateUUID();
    }
    return {};
}

std::optional<std::string> create2(bool flag) {
    return flag ? std::optional<std::string>(GenerateUUID()) : std::nullopt;
}

TEST(OptionalTest, BasicTest1) {
    auto c1 = create1(true);
    ASSERT_TRUE(c1.has_value());
    LOG(INFO) <<"create true, has_value: " << c1.has_value() <<" value: " << c1.value();

    auto c2 = create1(false);
    ASSERT_FALSE(c2.has_value());
    LOG(INFO) <<"create false, has_value: " << c2.has_value();
    ASSERT_EQ(c2.value_or(""), "");
}

