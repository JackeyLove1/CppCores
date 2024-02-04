#include <algorithm>
#include <cmath>
#include <random>
#include <bits/stdc++.h>
#include <glog/logging.h>
#include <gtest/gtest.h>
#include <omp.h>
#include "utils/time.h"

TEST(OMPTest, Test1) {
    constexpr auto NumThreads = 4u;
#pragma omp parallel num_threads(NumThreads)
    {
        int ID = omp_get_thread_num();
        LOG(INFO) << "hello: " << ID << " World: " << ID;
    }
}

TEST(OMPTest, Test2) {
    constexpr auto NumThreads = 4u;
#pragma omp parallel num_threads(NumThreads)
    {
        auto ID = omp_get_thread_num();
        auto num_threads = omp_get_num_threads();
        LOG(INFO) << "num_threads: " << num_threads << " ID: " << ID;
    }
}

TEST(OMPTest, Test3) {
    constexpr auto N = 250000u;
    TimeCost cost{};
    constexpr double k = 0.1;
    std::array<double, N> A{}, B{}, C{};
    A.fill(0.0), B.fill(0.1), C.fill(0.2);
    {
        for (size_t i = 0; i < N; ++i) {
            A[i] = sqrt(B[i] * k + C[i] * static_cast<double>(i));
        }
    }
    LOG(INFO) << "Normal cost: " << cost.ElapsedMs() << " ms";
    A.fill(0.0), B.fill(0.1), C.fill(0.2);
    cost.Reset();
#pragma omp parallel for
    for (size_t i = 0; i < N; ++i) {
        A[i] = sqrt(B[i] * k + C[i] * static_cast<double>(i));
    }
    LOG(INFO) << "Parallel1 cost: " << cost.ElapsedMs() << " ms";

    A.fill(0.0), B.fill(0.1), C.fill(0.2);
    cost.Reset();
#pragma omp parallel
    {
        const unsigned int num_threads = omp_get_num_threads();
        const unsigned int thread_id = omp_get_thread_num();

        const unsigned int items_per_thread = N / num_threads;
        const unsigned int start_index = thread_id * items_per_thread;
        unsigned int end_index = (thread_id == num_threads - 1) ? N : (thread_id + 1) * items_per_thread;

        for (size_t i = start_index; i < end_index; ++i) {
            A[i] = sqrt(B[i] * 0.1 + C[i] * static_cast<double>(i));
        }
    }
    LOG(INFO) << "Parallel2 cost: " << cost.ElapsedMs() << " ms";
}

TEST(OMPTest, Test4) {
    constexpr auto N = 500000u;
    std::array<uint32_t, N> A{};
    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_int_distribution<> dist(0, 100);
    std::generate(A.begin(), A.end(), [&]() { return dist(engine); });

    TimeCost cost{};
    uint64_t sum = 0, psum = 0;
#pragma omp parallel private(psum)
    {
        psum = 0;
#pragma omp for
        for (size_t i = 0; i < N; ++i) {
            psum = psum + A[i];
        }
#pragma omp critical
        {
            sum = sum + psum;
        };
    }
    LOG(INFO) << "Parallal 1 Cost: " << cost.Cost();

    cost.Reset();
    uint64_t sum2 = 0;
    for (size_t i = 0; i < N; ++i) {
        sum2 = sum2 + A[i];
    }
    LOG(INFO) << "Normal Cost: " << cost.Cost();
    ASSERT_EQ(sum, sum2);

    cost.Reset();
    uint64_t sum3 = 0;
#pragma  omp parallel for reduction (+:sum3)
    for (size_t i = 0; i < N; ++i) {
        sum3 = sum3 + A[i];

    }
    LOG(INFO) << "Parallel 2 Cost: " << cost.Cost();
    ASSERT_EQ(sum, sum3);
}

TEST(OMPTest, Test5) {
    constexpr auto N = 500000u;
    double ave = 0.0;
    std::array<double, N> A{};
    TimeCost cost{};
#pragma omp parallel for reduction (+:ave)
    for (size_t i = 0; i < N; ++i) {
        ave += A[i];
    }
    ave = ave / N;
    LOG(INFO) << "Parallel Cost: " << cost.Cost();
}

TEST(OMPTest, StaticTest1) {
    constexpr auto N = 1000000u;
    std::array<double, N> A{};
    auto InitA = [&A]() {
        for (size_t i = 0; i < N; ++i) {
            A[i] = 2.0 * static_cast<double >(i);
        }
    };
    InitA();
    TimeCost cost{};
    for (size_t i = 1; i < N; ++i) {
        A[i] = A[i] * sqrt(i) / pow(sin(i), tan(i));
    }
    LOG(INFO) << "Normal Cost: " << cost.Cost();

    InitA();
    cost.Reset();
#pragma omp parallel
    {
#pragma omp for schedule(static)
        for (size_t i = 1; i < N; ++i) {
            A[i] = A[i] * sqrt(i) / pow(sin(i), tan(i));
        }
    }
    LOG(INFO) << "Parallel Cost: " << cost.Cost();
}


TEST(OMPTest, DynamicTest) {
    constexpr auto N = 500000u;
    auto check_prime = [N](size_t num) {
        for (int i = 2; i <= static_cast<int>(sqrt(num)); ++i) {
            if (num % i == 0) {
                return false;
            }
        }
        return true;
    };

    int sum = 0;
    TimeCost cost{};
    for (size_t i = 2; i <= N; ++i) {
        if (check_prime(i)) ++sum;
    }
    LOG(INFO) << "Sequential Cost: " << cost.Cost();

    cost.Reset();
#pragma omp parallel
    {
#pragma omp for reduction(+:sum) schedule(static)
        for (size_t i = 2; i <= N; ++i) {
            if (check_prime(i)) ++sum;
        }
    }
    LOG(INFO) << "Parallel Static Cost: " << cost.Cost();

    cost.Reset();
#pragma omp parallel
    {
#pragma omp for reduction(+:sum) schedule(dynamic)
        for (size_t i = 2; i <= N; ++i) {
            if (check_prime(i)) ++sum;
        }
    }
    LOG(INFO) << "Parallel 1 Cost: " << cost.Cost();

    cost.Reset();
#pragma omp parallel
    {
#pragma omp for reduction(+:sum) schedule(dynamic, 2)
        for (size_t i = 2; i <= N; ++i) {
            if (check_prime(i)) ++sum;
        }
    }
    LOG(INFO) << "Parallel 2 Cost: " << cost.Cost();

    cost.Reset();
#pragma omp parallel
    {
#pragma omp for reduction(+:sum) schedule(dynamic, 4)
        for (size_t i = 2; i <= N; ++i) {
            if (check_prime(i)) ++sum;
        }
    }
    LOG(INFO) << "Parallel 3 Cost: " << cost.Cost();
}

TEST(OMPTest, PrivateAndShared) {
    int a = 10;
#pragma omp parallel for private(a)
    for (int i = 0; i < 4; ++i) {
        a = i + 5;
        printf("Private: Thread id = %d and a = %d\n", omp_get_thread_num(), a);
    }
    LOG(INFO) << "------------------------------";

    a = 10;
#pragma omp parallel for shared(a)
    for (int i = 0; i < 4; ++i) {
        a = i + 5;
        printf("Shared: Thread id = %d and a = %d\n", omp_get_thread_num(), a);
    }

    LOG(INFO) << "------------------------------";
    a = 10;
#pragma omp parallel for shared(a)
    for (int i = 0; i < 4; ++i) {
#pragma omp critical
        a = i + 5;
        printf("Shared: Thread id = %d and a = %d\n", omp_get_thread_num(), a);
    }
}