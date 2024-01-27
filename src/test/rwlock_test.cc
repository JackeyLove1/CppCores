#include <atomic>
#include <thread>
#include <shared_mutex>
#include <mutex>
#include <glog/logging.h>
#include <gtest/gtest.h>
#include "concurrent/rwlock.h"

TEST(RWLockTest, BasicTest1) {
    { PosixRWLock lock1(PosixRWLock::Kind::kKindDefault); }
    { PosixRWLock lock2(PosixRWLock::Kind::kKindPreferReader); }
    { PosixRWLock lock3(PosixRWLock::Kind::kKindPreferWriter); }
}

