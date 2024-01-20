#include <algorithm>
#include <array>
#include <random>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include "utils/utils.h"
#include "utils/crypto.h"
#include "utils/str_utils.h"

class CryptoTest : public ::testing::Test {
public:
    void SetUp() override {}

    void TearDown() override {}

public:
    static constexpr int32_t kBufferSize = 4097;
    static constexpr int32_t kKeySize = 32;
};

TEST_F(CryptoTest, DISABLED_AES256Test) {
    for (int i = 0; i < kBufferSize; ++i) {
        auto str = GenRandomString(i);
        std::string encode;
        encode.resize(i + 30);
        std::string decode;
        decode.resize(i + 30);

        auto key = GenRandomString(32);

        int32_t len = AESCipher::AES256Ecb(key.c_str(), str.c_str(), str.size(), &encode[0], true);
        LOG(INFO) << "encode_len: " << len;
        if (len > 0) {
            encode.resize(len);
        }
        len = AESCipher::AES256Ecb(key.c_str(), encode.c_str(), encode.size(), &decode[0], false);
        LOG(INFO) << "decode_len: " << len;
        if (len > 0) {
            decode.resize(len);
        }
        ASSERT_EQ(str, decode);
    }
}