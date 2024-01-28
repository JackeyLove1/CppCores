#include <gtest/gtest.h>
#include <glog/logging.h>
#include <list>
#include <bits/stdc++.h>

std::ostream &operator<<(std::ostream &os, const std::list<int> &list) {
    std::for_each(list.begin(), list.end(), [&](auto value) { os << value << " "; });
    return os;
}

TEST(ListTest, BasicTest) {
    std::list<int> list1{1, 2, 3, 4, 5};
    std::list<int> list2{10, 20, 30, 40, 50};

    LOG(INFO) << "list1: " << list1;
    LOG(INFO) << "list2: " << list2;

    auto it = list1.begin();
    std::advance(it, 2);

    list1.splice(it, list2);
    ASSERT_TRUE(list2.empty());
    auto list3 = list1;
    LOG(INFO) << "list1: " << list1;
    LOG(INFO) << "list2: " << list2;

    list2.splice(list2.end(), list1);
    LOG(INFO) << "list2: " << list2;
    ASSERT_TRUE(std::equal(list2.begin(), list2.end(), list3.begin()));
}
