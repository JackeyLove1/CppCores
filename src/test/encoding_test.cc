#include <algorithm>
#include <array>
#include <random>
#include <gtest/gtest.h>
#include "utils/utils.h"
#include "utils/encoding.h"

class Base64Test : public ::testing::Test {
public:
    void SetUp() override {}

    void TearDown() override {}
};

TEST_F(Base64Test, EncodeAndDecodeTest) {
    const std::string original_string = "Hello, World!";
    const std::string expected_string = "SGVsbG8sIFdvcmxkIQ==";
    std::string encoded = Base64Encoding(reinterpret_cast<const unsigned char *>(original_string.c_str()),
                                         original_string.length());
    std::string decoded = Base64Decoding(encoded);

    ASSERT_EQ(expected_string, encoded);
    ASSERT_EQ(original_string, decoded);
}