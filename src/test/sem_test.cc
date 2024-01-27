#include <atomic>
#include <thread>
#include <glog/logging.h>
#include <gtest/gtest.h>
#include "concurrent/sem.h"

class SemaphoreOpsTest : public ::testing::Test {
protected:
    sem_t semaphore{};

    void SetUp() override {
        sem_init(&semaphore, 0, 1);
    }

    void TearDown() override {
        sem_destroy(&semaphore);
    }
};

TEST_F(SemaphoreOpsTest, AcquireRelease) {
    // Test Acquire
    SemaphoreOps::Acquire(&semaphore);
    ASSERT_EQ(SemaphoreOps::GetValue(&semaphore), 0u);

    // Test Release
    SemaphoreOps::Release(&semaphore);
    ASSERT_EQ(SemaphoreOps::GetValue(&semaphore), 1u);
}

TEST_F(SemaphoreOpsTest, TryAcquire) {
    // The semaphore should acquire successfully
    ASSERT_TRUE(SemaphoreOps::TryAcquire(&semaphore));
    ASSERT_EQ(SemaphoreOps::GetValue(&semaphore), 0u);

    // The semaphore should now not be able to be acquired
    ASSERT_FALSE(SemaphoreOps::TryAcquire(&semaphore));
}

TEST_F(SemaphoreOpsTest, TimedAcquire) {
    // Acquire the semaphore
    SemaphoreOps::Acquire(&semaphore);

    // Attempt to acquire the semaphore with a timeout, which should fail.
    ASSERT_FALSE(SemaphoreOps::TimedAcquire(&semaphore, 500)); // 500 microsecond timeout

    // Release the semaphore, which would allow us to acquire it again.
    SemaphoreOps::Release(&semaphore);

    // Now it should succeed within the timeout.
    ASSERT_TRUE(SemaphoreOps::TimedAcquire(&semaphore, 500));
}

TEST(SemaphoreTest, AcquireRelease){
    Semaphore sem(1);
    ASSERT_NO_THROW(sem.Acquire());
    ASSERT_NO_THROW(sem.Release());

    ASSERT_TRUE(sem.TryAcquire());
}

TEST(SemaphoreTest, TimedAcquire){
    Semaphore sem(1);
    sem.Acquire();
    ASSERT_FALSE(sem.TimedAcquire(50));
}