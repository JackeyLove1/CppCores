#include <algorithm>
#include <random>
#include <gtest/gtest.h>
#include "utils/utils.h"
#include "concurrent/closure.h"

TEST(ClosureTest, BaseTest1) {
    // Arrange
    std::string str1 = "Hello";
    std::string str2 = ", World";
    auto Concatenate = [](auto s1, auto s2) {
        return std::move(s1) + std::move(s2);
    };
    auto closure = Closure<std::string, std::string, std::string>{Concatenate, str1, str2};
    std::string result = closure.Run();
    ASSERT_EQ(result, "Hello, World");
}

TEST(Closure, BaseTest2) {
    auto f1 = [](int a, int b) { return a + b; };
    auto c1 = make_closure(f1, 1, 2);
    ASSERT_EQ(c1(), f1(1, 2));

    auto c2 = make_closure([]() { LOG(INFO) << "print"; });
    c2();

    int a = 0;
    auto c3 = make_closure([&]() { a = 1; });
    c3();
    ASSERT_NE(a, 1);

}