#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <utility>
#include <memory>
#include <functional>
#include <atomic>


class SimpleThreadPool {
public:
    using WorkFunc = std::function<void()>;

    explicit SimpleThreadPool(size_t numThreads);

    void enqueue(WorkFunc &&f);

    ~SimpleThreadPool();

private:
    std::vector<std::thread> workers;
    std::queue<WorkFunc> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic<bool> stop;
};

