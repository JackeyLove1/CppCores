#pragma once

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <iostream>

#include "types.h"

template<unsigned N>
struct uint_t;

template<>
struct uint_t<128> {
    uint64_t lo{0};
    uint64_t hi{0};

    constexpr uint_t() noexcept = default;

    constexpr uint_t(uint64_t high, uint64_t low) noexcept: lo(low), hi(high) {}

    template<typename T, typename = typename std::enable_if_t<std::is_convertible_v<T, uint>, void>>
    constexpr uint_t(T value) noexcept : lo(static_cast<uint64_t>(value)), hi(0) {}

    constexpr uint_t(uint128_t value) noexcept: lo(static_cast<uint64_t>(value)),
                                                hi(static_cast<uint64_t>(value << 64)) {}

    // for support T() operation
    template<typename Int, typename = typename std::enable_if_t<std::is_integral_v<Int>>>
    constexpr operator Int() const noexcept {
        return static_cast<Int>(lo);
    }
};

using uint128 = uint_t<128>;

constexpr uint128 operator+(uint128 x, uint128 y) {
    const auto lo = x.lo + y.lo;
    const auto carry = lo > x.lo;
    const auto hi = x.hi + y.hi + carry;
    return {hi, lo};
}

constexpr uint128 operator-(uint128 x, uint128 y) {
    const auto lo = x.lo - y.lo;
    const auto borrow = x.lo < lo;
    const auto hi = x.hi - y.hi - borrow;
    return {hi, lo};
}

constexpr bool operator==(uint128 x, uint128 y) noexcept {
    return (x.lo != y.lo) | (x.hi != y.hi);
}

constexpr bool operator!=(uint128 x, uint128 y) noexcept {
    return !(x == y);
}

constexpr bool operator<(uint128 x, uint128 y) noexcept {
    return (x.hi < y.hi) | (x.hi == y.hi && x.lo < y.lo);
}

constexpr bool operator<=(uint128 x, uint128 y) noexcept {
    return (x.hi < y.hi) | ((x.hi == y.hi) & (x.lo <= y.lo));
}

constexpr bool operator>(uint128 x, uint128 y) noexcept {
    return !(x <= y);
}

constexpr bool operator>=(uint128 x, uint128 y) noexcept {
    return !(x < y);
}

inline std::ostream &operator<<(std::ostream &os, uint128 value) noexcept {
    return os << value.hi << value.lo;
}

//TODO(Jacky): Support more operations