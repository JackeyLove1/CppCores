#include "thread_pool.h"

SimpleThreadPool::SimpleThreadPool(size_t numThreads) : stop{false} {
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back([this] {
            for (;;) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->queueMutex);
                    this->condition.wait(lock, [this] {
                        return this->stop.load(std::memory_order_relaxed) || !this->tasks.empty();
                    });
                    if (this->stop.load(std::memory_order_relaxed)) {
                        return;
                    }
                    if (this->tasks.empty()) {
                        using namespace std::chrono_literals;
                        this->condition.wait_for(lock, 100us,
                                                 [this] {
                                                     return this->stop.load(std::memory_order_relaxed) ||
                                                            !this->tasks.empty();
                                                 });
                    }
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }
                std::invoke(task);
            }
        });
    }
}

void SimpleThreadPool::enqueue(WorkFunc &&f) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        tasks.emplace(std::forward<WorkFunc>(f));
    }
    condition.notify_one();
}


SimpleThreadPool::~SimpleThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop.store(true, std::memory_order_relaxed);
    }
    condition.notify_all();
    for (auto &&worker: workers) {
        worker.join();
    }
}