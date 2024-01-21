#pragma once

#include <iostream>
#include <algorithm>
#include <vector>
#include <random>
#include <type_traits>
#include <glog/logging.h>

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

/**
 * @brief 获取当前的调用栈
 * @param[out] bt 保存调用栈
 * @param[in] size 最多返回层数
 * @param[in] skip 跳过栈顶的层数
 */
void Backtrace(std::vector<std::string>& bt, int size = 64, int skip = 1);

/**
 * @brief 获取当前栈信息的字符串
 * @param[in] size 栈的最大层数
 * @param[in] skip 跳过栈顶的层数
 * @param[in] prefix 栈信息前输出的内容
 */
std::string BacktraceToString(int size = 64, int skip = 2, const std::string& prefix = "");