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

TEST(HeapTest, BasicTest) {
    std::vector<int> v{3, 1, 4, 1, 5, 9};
    auto vector_to_string = [&v]() {
        std::string result;
        for (const auto &value: v) {
            result += std::to_string(value) + " ";
        }
        return result;
    };
    LOG(INFO) << "Vec: " << vector_to_string();
    std::make_heap(v.begin(), v.end());
    LOG(INFO) << "Make Heap: " << vector_to_string();
    v.push_back(6);
    LOG(INFO) << "After push: " << vector_to_string();
    std::push_heap(v.begin(), v.end());
    LOG(INFO) << "After push heap: " << vector_to_string();
    std::pop_heap(v.begin(), v.end());
    LOG(INFO) << "after pop_heap:  " << vector_to_string();
    int largest = v.back();
    LOG(INFO) << "largest element: " << largest;
}
