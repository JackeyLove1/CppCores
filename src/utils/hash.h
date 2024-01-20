#pragma once

#include <algorithm>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <string>
#include <string_view>
#include <openssl/md5.h>
#include <openssl/sha.h>

#define    ROTL(x, r) ((x << r) | (x >> (32 - r)))

static inline uint32_t fmix32(uint32_t h) {
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;

    return h;
}

uint32_t murmur3_hash(const char *str, uint32_t seed = 1060627423);

uint32_t murmur3_hash(std::string_view str, uint32_t seed = 1060627423);

uint64_t murmur3_hash64(const char *str, uint32_t seed = 1060627423, uint32_t seed2 = 1050126127);

uint32_t murmur3_hash(const void *str, uint32_t size, uint32_t seed = 1060627423);

uint64_t murmur3_hash64(const void *str, uint32_t size, uint32_t seed = 1060627423,
                        uint32_t seed2 = 1050126127);

uint32_t quick_hash(const char *str);

uint32_t quick_hash(const std::string &str);

uint32_t quick_hash(std::string_view str);

uint32_t quick_hash(const void *str, uint32_t size);

std::string base64decode(const std::string &src);

std::string base64encode(const std::string &data);

std::string base64encode(const void *data, size_t len);

std::string md5(const std::string &data);

std::string sha1(const std::string &data);

// Returns result in blob
std::string md5sum(const std::string &data);

std::string md5sum(std::string_view data);

std::string md5sum(const void *data, size_t len);

std::string sha0sum(const std::string &data);

std::string sha0sum(std::string_view data);

std::string sha0sum(const void *data, size_t len);

std::string sha1sum(const std::string &data);

std::string sha1sum(std::string_view data);

std::string sha1sum(const void *data, size_t len);

std::string hmac_md5(const std::string &text, const std::string &key);

std::string hmac_sha1(const std::string &text, const std::string &key);

std::string hmac_sha256(const std::string &text, const std::string &key);

/// Output must be of size len * 2, and will *not* be null-terminated
void hexstring_from_data(const void *data, size_t len, char *output);

std::string hexstring_from_data(const void *data, size_t len);

std::string hexstring_from_data(const std::string &data);

std::string hexstring_from_data(std::string_view data);

/// Output must be of size length / 2, and will *not* be null-terminated
/// std::invalid_argument will be thrown if hexstring is not hex
void data_from_hexstring(const char *hexstring, size_t length, void *output);

std::string data_from_hexstring(const char *hexstring, size_t length);

std::string data_from_hexstring(const std::string &data);
