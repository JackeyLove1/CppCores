#include <algorithm>
#include <thread>
#include <vector>
#include <pthread.h>
#include <gtest/gtest.h>
#include "macro.h"
#include "work_stealing_queue.h"

using value_type = size_t;
bool g_stop = false;
constexpr size_t N = 1024 * 512;
constexpr size_t CAP = 8;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *steal_thread(void *arg) {
    std::vector<value_type> *stolen = new std::vector<value_type>;
    stolen->reserve(N);
    WorkStealingQueue<value_type> *q =
            (WorkStealingQueue<value_type> *) arg;
    value_type val;
    while (!g_stop) {
        if (q->steal(&val)) {
            stolen->push_back(val);
        } else {
            asm volatile("pause\n": : :"memory");
        }
    }
    return stolen;
}

void *push_thread(void *arg) {
    size_t npushed = 0;
    value_type seed = 0;
    WorkStealingQueue<value_type> *q =
            (WorkStealingQueue<value_type> *) arg;
    while (true) {
        pthread_mutex_lock(&mutex);
        const bool pushed = q->push(seed);
        pthread_mutex_unlock(&mutex);
        if (pushed) {
            ++seed;
            if (++npushed == N) {
                g_stop = true;
                break;
            }
        }
    }
    return nullptr;
}

void *pop_thread(void *arg) {
    std::vector<value_type> *popped = new std::vector<value_type>;
    popped->reserve(N);
    WorkStealingQueue<value_type> *q =
            (WorkStealingQueue<value_type> *) arg;
    while (!g_stop) {
        value_type val;
        pthread_mutex_lock(&mutex);
        const bool res = q->pop(&val);
        pthread_mutex_unlock(&mutex);
        if (res) {
            popped->push_back(val);
        }
    }
    return popped;
}


TEST(WSQTest, sanity) {
    WorkStealingQueue<value_type> q;
    ASSERT_EQ(0, q.init(CAP));
    pthread_t rth[8];
    pthread_t wth, pop_th;
    for (size_t i = 0; i < ARRAY_SIZE(rth); ++i) {
        ASSERT_EQ(0, pthread_create(&rth[i], nullptr, steal_thread, &q));
    }
    ASSERT_EQ(0, pthread_create(&wth, nullptr, push_thread, &q));
    ASSERT_EQ(0, pthread_create(&pop_th, nullptr, pop_thread, &q));

    std::vector<value_type> values;
    values.reserve(N);
    size_t nstolen = 0, npopped = 0;
    for (size_t i = 0; i < ARRAY_SIZE(rth); ++i) {
        std::vector<value_type> *res = nullptr;
        pthread_join(rth[i], (void **) &res);
        for (size_t j = 0; j < res->size(); ++j, ++nstolen) {
            values.push_back((*res)[j]);
        }
    }
    pthread_join(wth, nullptr);
    std::vector<value_type> *res = nullptr;
    pthread_join(pop_th, (void **) &res);
    for (size_t j = 0; j < res->size(); ++j, ++npopped) {
        values.push_back((*res)[j]);
    }

    value_type val;
    while (q.pop(&val)) {
        values.push_back(val);
    }

    std::sort(values.begin(), values.end());
    values.resize(std::unique(values.begin(), values.end()) - values.begin());

    ASSERT_EQ(N, values.size());
    for (size_t i = 0; i < N; ++i) {
        ASSERT_EQ(i, values[i]);
    }
    LOG(INFO) << "stolen=" << nstolen
              << " popped=" << npopped
              << " left=" << (N - nstolen - npopped);
}

TEST(WSQTest, STDThread) {
    const size_t steal_worker = 8;
    const size_t N = 1024 * 512;
    std::vector<std::vector<int>> steals(steal_worker, std::vector<int>());
    std::vector<std::thread> steal_threads;
    std::vector<size_t> pops;
    WorkStealingQueue<size_t> q;
    std::atomic<bool> stop{false};
    ASSERT_EQ(0, q.init(CAP));
    for (auto i = 0u; i < steal_worker; ++i) {
        steal_threads.emplace_back([&steals, &q, &stop, i]() {
            while (!stop.load(std::memory_order_relaxed)) {
                size_t val;
                if (q.steal(&val)) {
                    steals[i].push_back(val);
                } else {
                    std::this_thread::yield();
                }
            }
        });
    }
    std::thread pop_worker([&pops, &q, &stop]() {
        while (!stop.load(std::memory_order_relaxed)) {
            size_t val;
            if (q.steal(&val)) {
                pops.push_back(val);
            }
        }
    });
    std::thread push_worker([&q, &stop, N]() {
        size_t seed = 0, npushed = 0;
        for (;;) {
            const bool pushed = q.push(seed);
            if (pushed) {
                ++seed;
                if (++npushed == N) {
                    stop.store(true, std::memory_order_release);
                    break;
                }
            }
        }
    });
    push_worker.join();
    pop_worker.join();
    for (auto &&t: steal_threads) {
        t.join();
    }
    std::vector<size_t> values;
    values.swap(pops);
    for (auto &&steal: steals) {
        values.insert(values.end(), steal.begin(), steal.end());
    }
    std::sort(values.begin(), values.end());
    values.erase(std::unique(values.begin(), values.end()), values.end());
    ASSERT_EQ(values.size(), N);
    for (size_t i = 0; i < N; ++i) {
        ASSERT_EQ(i, values[i]);
    }
}