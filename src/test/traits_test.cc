#include <algorithm>
#include <type_traits>
#include <chrono>
#include <bits/stdc++.h>
#include <gtest/gtest.h>
#include <glog/logging.h>

template<class... Args>
auto Rorder(Args &&...args) {
    std::vector<std::common_type_t<Args...>> vec;
    bool ok = true;
    (ok= ... = (vec.push_back(args), true));
    return vec;
}

TEST(RorderTest, Test1) {
    std::vector<int> vec = Rorder(1, 2, 3, 4, 5, 6, 'a');
    ASSERT_EQ(vec.front(), static_cast<int>('a'));
}

template<typename T, typename S>
constexpr auto durationDiff(const T &t, const S &s) -> typename std::common_type_t<T, S> {
    typedef typename std::common_type_t<T, S> Common;
    return Common(t) - Common(s);
}

TEST(DurationDiffTest, Test1) {
    using namespace std::literals;
    constexpr auto ms = 30ms;
    constexpr auto us = 1100us;
    constexpr auto diff = durationDiff(ms, us);
    ASSERT_EQ(diff.count(), 28900u);
}

template<typename T1, typename T2>
std::common_type_t<T1, T2> CurMin(const T1 &lhs, const T2 &rhs) {
    return lhs < rhs ? lhs : rhs;
}

TEST(CurMinTest, Test1) {
    auto r1 = CurMin(11.7, 3.5);
    auto r2 = CurMin(7.1, 9);
    ASSERT_FLOAT_EQ(r1, 3.5);
    ASSERT_FLOAT_EQ(r2, 7.1);
}