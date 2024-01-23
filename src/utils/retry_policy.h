#pragma once

#include <cstdlib>
#include <cstdint>
#include <algorithm>

class RetryPolicy {
public:
    RetryPolicy() = default;

    virtual ~RetryPolicy() = default;

    virtual int32_t GetRetryInterval(uint32_t retry_count) = 0;
};

class ExponentialRetryPolicy : public RetryPolicy {
public:
    explicit ExponentialRetryPolicy(int32_t base_interval_ms, int32_t max_interval_ms) :
            base_interval_ms_{base_interval_ms},
            max_interval_ms_{max_interval_ms} {}

    int32_t GetRetryInterval(uint32_t retry_count) override {
        return std::min(max_interval_ms_, base_interval_ms_ * (1 << std::min(retry_count, 31u)));
    }

private:
    int32_t base_interval_ms_{};
    int32_t max_interval_ms_{};
};