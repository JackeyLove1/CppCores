#pragma once

#include <ctime>
#include <chrono>
#include <thread>

/**
 * @note
 * "ms" stands for milliseconds (1 millisecond = 1/1000 seconds)
 * "us" or sometimes "µs" stands for microseconds (1 microsecond = 1/1,000,000 seconds)
 * "ns" stands for nanoseconds (1 nanoseconds = 1/1,000,000,000 seconds)
 */
static inline auto GetCurrentTimeInNs() {
    auto now = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
}

static inline auto GetCurrentTimeInUs() {
    auto now = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(now).count();
}

static inline auto GetCurrentTimeInMs() {
    auto now = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
}

class TimeCost {
public:
    explicit TimeCost() : start_{GetCurrentTimeInNs()} {}

    auto ElapsedNs() const { return GetCurrentTimeInNs() - start_; }

    auto ElapsedUs() const { return ElapsedNs() / 1000; }

    auto ElapsedMs() const { return ElapsedUs() / 1000; }

    auto ElapsedSec() const { return ElapsedMs() / 1000; }

    void Reset() { start_ = GetCurrentTimeInNs(); }

private:
    decltype(GetCurrentTimeInNs()) start_{};
};

static inline void SleepInSec(int64_t sec) {
    std::this_thread::sleep_for(std::chrono::seconds(sec));
}

static inline void SleepInMs(int64_t ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

static inline void SleepInUs(int64_t us) {
    std::this_thread::sleep_for(std::chrono::microseconds(us));
}