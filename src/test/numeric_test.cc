#include <utility>
#include <memory>
#include <functional>
#include <numeric>
#include <glog/logging.h>
#include <gtest/gtest.h>
#include "meta/integer_sequence.h"
#include "meta/simple_tuple.h"


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

TEST(IntegerSequenceTest, Test1) {
    print_sequence(integer_sequence<unsigned, 1, 2, 3, 4>{});
    print_sequence(make_integer_sequence<int, 10>{});
    print_sequence(make_index_sequence<10>{});
}

TEST(SimpleTupleTest, Test1) {
    int a = 1;
    double b = 2.3;
    size_t c = 4u;
    std::string d = "56789";
    Tuple my_tuple(a, b, c, d);

    EXPECT_EQ(tuple_get<0>(my_tuple), a);
    EXPECT_EQ(tuple_get<1>(my_tuple), b);
    EXPECT_EQ(tuple_get<2>(my_tuple), c);
    EXPECT_EQ(tuple_get<3>(my_tuple), d);

    EXPECT_EQ(my_tuple.get<0>(), a);
    EXPECT_EQ(my_tuple.get<1>(), b);
    EXPECT_EQ(my_tuple.get<2>(), c);
    EXPECT_EQ(my_tuple.get<3>(), d);

    print_tuple(std::cout, my_tuple);
}

TEST(SimpleTupleTest, HashTest) {
    auto tuple1 = make_tuple<int, double, std::string>(1, 2.3, "456");
    auto hash1 = hash_tuple_struct(tuple1);
    LOG(INFO) << HashTuple<Tuple<int, double, std::string>>{}(tuple1);
    LOG(INFO) << hash_tuple_struct(tuple1);
    LOG(INFO) << hash_tuple_fold(tuple1);

    auto tuple2 = make_tuple<int, double, std::string>(1, 2.3, "4567");
    auto hash2 = hash_tuple_struct(tuple2);
    ASSERT_NE(hash1, hash2);
}