#include <algorithm>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include "utils/simple_ostream.h"


class SimpleOStreamTest : public ::testing::Test {
public:
    void SetUp() override {}

    void TearDown() override {}
};

TEST_F(SimpleOStreamTest, EncodeTest) {
    std::ostringstream os;
    EXPECT_TRUE(EncodeToStream(os, 1, 2u, 3.4f, "Hello"));
}

TEST_F(SimpleOStreamTest, DISABLED_DecodeTest){
    // TODO(Jacky): fix me, this test will cause coredump
    std::stringstream ss;
    int i_in = 42;
    float f_in = 3.14f;
    std::string s_in = "hello world";
    EXPECT_TRUE(EncodeToStream(ss, i_in, f_in, s_in));
    int i_out;
    float f_out;
    std::string s_out;
    EXPECT_TRUE(DecodeFromStream(ss, &i_out, &f_out, &s_out));

    EXPECT_EQ(i_in, i_out);
    EXPECT_EQ(f_in, f_out);
    EXPECT_EQ(s_in, s_out);
}