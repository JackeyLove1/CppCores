#pragma once

#include <iostream>
#include <algorithm>
#include <vector>
#include <random>
#include <type_traits>

static std::random_device DefaultRandomDevice{};
static std::mt19937 DefaultGenerator(DefaultRandomDevice());


static inline std::mt19937 GetRandomEngine(uint32_t seed) {
    return std::mt19937{seed};
}

static inline std::mt19937 GetDefaultRandomEngine() {
    return DefaultGenerator;
}

static inline std::mt19937 GetRandomEngine() {
    return std::mt19937{DefaultRandomDevice()};
}

template<typename T, typename std::enable_if_t<std::is_integral_v<T>, void *> = nullptr>
static inline T GenRandom() {
    static std::uniform_int_distribution<T> distribution{};
    return distribution(DefaultGenerator);
}