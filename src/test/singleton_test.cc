#include <algorithm>
#include <random>
#include <thread>
#include <type_traits>
#include <gtest/gtest.h>
#include "utils/utils.h"
#include "utils/singleton.h"


class SingletonTest : public ::testing::Test {
public:
    void SetUp() override {}

    void TearDown() override {}

};

template<typename T>
class SingletonTestClass : public SingletonBase<SingletonTestClass<T>> {
public:
    friend class SingletonBase<SingletonTestClass<T>>;

    explicit SingletonTestClass(T value) : value_{std::move(value)} {}

    auto GetValue() const {
        return value_;
    }

private:
    T value_{};
};

template<typename T>
void singleton_test(T t) {
    auto *instance1 = SingletonTestClass<T>::GetInstance(t);
    auto *instance2 = SingletonTestClass<T>::GetInstance(t);
    EXPECT_EQ(instance1, instance2);
    EXPECT_EQ(instance1->GetValue(), t);
    EXPECT_EQ(instance2->GetValue(), t);
}

TEST_F(SingletonTest, BasicTest) {
    int value1 = 1;
    singleton_test(value1);

    std::string value2 = "hello";
    singleton_test(value2);

}

TEST_F(SingletonTest, MultiThreadTest) {
    auto *instance = SingletonTestClass<int>::GetInstance(1);
    std::vector<std::thread> threads;
    size_t num_threads = 8;
    for (size_t i = 0; i < num_threads; ++i) {
        threads.emplace_back([instance]() {
            int number = GenRandom<int>();
            auto *thread_instance = SingletonTestClass<int>::GetInstance(number);
            EXPECT_EQ(thread_instance, instance);
            EXPECT_EQ(thread_instance->GetValue(), instance->GetValue());
        });
    }
    for (auto &&thread: threads) {
        thread.join();
    }
}