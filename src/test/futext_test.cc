#include <atomic>
#include <numeric>
#include <thread>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include <pthread.h>

#include "sys_futex.h"
#include "mutex.h"
#include "macro.h"
#include "utils/time.h"

volatile bool stop = false;
std::atomic<int> nthread(0);

void *read_thread(void * arg){
    auto* m = reinterpret_cast<std::atomic<int>*> (arg);
    int njob = 0;
    while (!stop){
        int x ;
        while (!stop && (x = m->load(std::memory_order_acquire)) != 0){
            if (x > 0){
                while ((x = m->fetch_sub(1)) > 0){
                    ++njob;
                    const auto start = GetCurrentTimeInNs();
                    while (GetCurrentTimeInNs() < start + 1000){

                    }
                    if (stop){
                        return new int(njob);
                    }
                }
                m->fetch_and(1);
            }else {
                cpu_relax();
            }
        }
    }
    ++nthread;
    futex_wait_private(m/*lock1*/, 0/*consumed_njob*/, nullptr);
    --nthread;
    return new int(njob);
}

TEST(FutextTest, DISABLED_Basic){
    constexpr size_t N = 100000;
    std::atomic<int> lock1(0);
    pthread_t rth[8];
    for (size_t i = 0; i < ARRAY_SIZE(rth); ++i) {
        ASSERT_EQ(0, pthread_create(&rth[i], NULL, read_thread, &lock1));
    }

    const auto t1 = GetCurrentTimeInNs();
    for (auto i = 0u; i < N; ++i){
        if (nthread){
            lock1.fetch_and(1, std::memory_order_acq_rel);
            futex_wake_private(&lock1, 1);
        } else {
            lock1.fetch_and(1, std::memory_order_acq_rel);
            if (nthread){
                futex_wake_private(&lock1, 1);
            }
        }
    }
    const auto t2 = GetCurrentTimeInNs();

    SleepInUs(3000000);
    stop = true;
    for (int i = 0; i < 10; ++i) {
        futex_wake_private(&lock1, std::numeric_limits<int>::max());
        sched_yield();
    }
    int njob = 0;
    int* res;
    for (size_t i = 0; i < ARRAY_SIZE(rth); ++i) {
        pthread_join(rth[i], (void**)&res);
        njob += *res;
        delete res;
    }
    printf("wake %lu times, %" PRId64 "ns each, lock1=%d njob=%d\n",
           N, (t2-t1)/N, lock1.load(), njob);
    ASSERT_EQ(N, (size_t)(lock1.load() + njob));
}