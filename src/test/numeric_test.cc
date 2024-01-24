#include <utility>
#include <memory>
#include <functional>
#include <numeric>
#include <glog/logging.h>
#include <gtest/gtest.h>


TEST(AccumulationTest, BasicTest) {
    int base = 0;
    std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int sum = std::accumulate(v.begin(), v.end(), base, std::plus<int>());
    int product = std::accumulate(v.begin(), v.end(), 1, std::multiplies<int>());
    EXPECT_EQ(sum, 55);
    EXPECT_EQ(product, 3628800);

    auto fold = [](std::string s, int value) {
        return std::move(s) + "-" + std::to_string(value);
    };
    std::string s = std::accumulate(std::next(v.begin()), v.end(), std::to_string(v.front()), fold);
    std::string result = "1-2-3-4-5-6-7-8-9-10";
    EXPECT_EQ(s, result);
}