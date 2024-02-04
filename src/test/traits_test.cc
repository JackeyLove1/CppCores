#include <algorithm>
#include <type_traits>
#include <chrono>
#include <memory>
#include <atomic>
#include <string_view>
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

struct T {
    enum {
        int_t, float_t
    } m_type;

    template<typename Integer,
            typename std::enable_if_t<std::is_integral_v<Integer>, int> = 0>
    explicit T(Integer): m_type(int_t) {};

    template<typename Floating,
            typename std::enable_if_t<std::is_floating_point_v<Floating>, int> = 0>
    explicit T(Floating): m_type(float_t) {};
};

TEST(EnableIfTest, Test1) {
    T t1(1u);
    ASSERT_EQ(t1.m_type, T::int_t);
    T t2(1.1f);
    ASSERT_EQ(t2.m_type, T::float_t);
}

template<size_t N>
struct IsPower2 {
    static_assert(N > 8, "N must greater than 8");
    static_assert((N & (N - 1)) == 0, "N must be power of 2");
};


TEST(Power2Test, Test1) {
    [[maybe_unused]]IsPower2<16> is_power1;
    [[maybe_unused]]IsPower2<32> is_power2;
}

template<typename T>
struct Node {
    T data;
    Node *next;

    explicit Node(const T &data) : data(data), next(nullptr) {}
};

template<typename T>
struct Stack {
    std::atomic<Node<T> *> head;
public:
    void push(const T &data) {
        auto *new_node = new Node<T>(data);
        new_node->next = head;
        while (!head.compare_exchange_weak(new_node->next,
                                           new_node,
                                           std::memory_order_release,
                                           std::memory_order_relaxed));
    }
};

TEST(LockFreeStackTest, Test1) {
    Stack<int> s{};
    s.push(1);
    s.push(2);
    s.push(3);
}

// dynamic refractor
#ifdef _WIN64
#define __PFUNC__ __FUNCSIG__
#else
#define __PFUNC__  __PRETTY_FUNCTION__
#endif
enum class DataType : uint8_t {
    USB, PCI, HD, NOT
};

constexpr auto DataTypeSize = static_cast<size_t>(DataType::NOT);

template<auto T>
constexpr std::string_view TypeInfo() {
    return __PFUNC__;
}

template<typename T>
constexpr auto enumMapName(const T &t) {
    constexpr std::array<std::string_view, DataTypeSize> type_names = {
            []<std::size_t ... Is>(std::index_sequence<Is...>){
                return std::array<std::string_view, DataTypeSize>{TypeInfo<static_cast<DataType>(Is)>()...};
            }(std::make_index_sequence<DataTypeSize>{})
    };
    return type_names[static_cast<size_t>(t)];
}

TEST(EnumMapTest, Test) {
    for (auto i = 0u; i < DataTypeSize; ++i) {
        LOG(INFO) << enumMapName(static_cast<DataType>(i));
    }
}