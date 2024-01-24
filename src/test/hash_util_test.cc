#include <algorithm>
#include <random>
#include <unordered_set>
#include <gtest/gtest.h>
#include "utils/hash.h"
#include "utils/str_utils.h"

class HashUtilsTest : public testing::Test {
public:
    void SetUp() override {}

    void TearDown() override {}

public:
    using HashFunc = std::function<std::string(const std::string &)>;

    static void CheckHashCrash(const HashFunc &hash) {
        std::unordered_set<std::string> results(kSize);
        for (uint32_t i = 0; i < kSize; ++i) {
            results.insert(std::invoke(hash, GenRandomString(kSize)));
        }
        ASSERT_EQ(kSize, results.size());
    }

public:
    static constexpr size_t kSize = 1000;
};

TEST_F(HashUtilsTest, MD5Test) {
    const std::string s{GenRandomString(kSize)};
    const std::string_view sv{s};
    auto md5_1 = md5sum(s);
    auto md5_2 = md5sum(sv);
    auto md5_3 = md5sum(s.c_str(), s.size());
    ASSERT_TRUE(md5_1 == md5_2 && md5_2 == md5_3);

    CheckHashCrash([](const std::string &s) { return md5sum(s); });
}

TEST_F(HashUtilsTest, MurmurHashTest) {
    const std::string s{GenRandomString(kSize)};
    const std::string_view sv{s};
    auto md5_1 = murmur3_hash(s);
    auto md5_2 = murmur3_hash(sv);
    auto md5_3 = murmur3_hash(s.c_str());
    ASSERT_TRUE(md5_1 == md5_2 && md5_2 == md5_3);

    CheckHashCrash([](const std::string &s) { return std::to_string(murmur3_hash(s)); });
}

TEST_F(HashUtilsTest, SHA0Test) {
    CheckHashCrash([](const std::string &s) { return sha0sum(s); });
}

TEST_F(HashUtilsTest, SHA1Test) {
    CheckHashCrash([](const std::string &s) { return sha1sum(s); });
}


TEST_F(HashUtilsTest, HmacTest) {
    const std::string public_key{};
    CheckHashCrash([public_key](const std::string &s) { return hmac_md5(s, public_key); });
    CheckHashCrash([public_key](const std::string &s) { return hmac_sha1(s, public_key); });
    CheckHashCrash([public_key](const std::string &s) { return hmac_sha256(s, public_key); });
}

TEST_F(HashUtilsTest, SignedUnsignedIssue) {
    const uint8_t data1[1] = {0x62};
    const uint8_t data2[2] = {0xc3, 0x97};
    const uint8_t data3[3] = {0xe2, 0x99, 0xa5};
    const uint8_t data4[4] = {0xe1, 0x80, 0xb9, 0x32};
    const uint8_t data5[48] = {
            0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
            0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x18, 0x28, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };

    ASSERT_EQ(Hash(0, 0, 0xbc9f1d34), 0xbc9f1d34);
    ASSERT_EQ(
            Hash(reinterpret_cast<const char *>(data1), sizeof(data1), 0xbc9f1d34),
            0xef1345c4);
    ASSERT_EQ(
            Hash(reinterpret_cast<const char *>(data2), sizeof(data2), 0xbc9f1d34),
            0x5b663814);
    ASSERT_EQ(
            Hash(reinterpret_cast<const char *>(data3), sizeof(data3), 0xbc9f1d34),
            0x323c078f);
    ASSERT_EQ(
            Hash(reinterpret_cast<const char *>(data4), sizeof(data4), 0xbc9f1d34),
            0xed21633a);
    ASSERT_EQ(
            Hash(reinterpret_cast<const char *>(data5), sizeof(data5), 0x12345678),
            0xf333dabb);
}