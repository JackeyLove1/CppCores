#include <algorithm>
#include <random>
#include <gtest/gtest.h>
#include "utils/utils.h"
#include "utils/memory_utils.h"


class MemoryUtilTest : public ::testing::Test {
public:
    void SetUp() override {}

    void TearDown() override {}
};

TEST_F(MemoryUtilTest, DefaultDeleterTest) {
    ScopedPtr<int> scoped_ptr(new int{100});
    EXPECT_EQ(*scoped_ptr, 100);
    *scoped_ptr = 101;
    EXPECT_EQ(*scoped_ptr, 101);
}

TEST_F(MemoryUtilTest, ArratDeleterTest) {
    ArrayPtr<int[]> array_ptr(new int[10]);
    for (int i = 0; i < 10; ++i) {
        array_ptr[i] = i;
        EXPECT_EQ(array_ptr[i], i);
    }
}

TEST_F(MemoryUtilTest, MallocDeleterTest) {
    int *raw_ptr = (int *) malloc(sizeof(int));
    *raw_ptr = 123;
    MallocPtr<int> mallocPtr(raw_ptr);
    EXPECT_EQ(*mallocPtr, 123);
}