#pragma once

#include <iostream>
#include <algorithm>
#include <vector>
#include <random>

static inline std::mt19937 GetRandomEngine() {
    std::random_device rd{};
    return std::mt19937{rd()};
}
