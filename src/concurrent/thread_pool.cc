#include <list>
#include <algorithm>
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

struct ToftThreadPool::Task {
    explicit Task(TaskFunc task) : func(std::move(task)) {}

    TaskNode link;
    TaskFunc func;
};

struct ToftThreadPool::ThreadContext {
    ThreadContext() : thread{nullptr}, exit{false} {};

    std::unique_ptr<std::thread> thread;
    mutable std::mutex mutex;
    std::condition_variable cond;
    TaskList pending_tasks;
    TaskList working_tasks;
    bool exit;

    bool GetPendingTask();
}__attribute__((aligned(64))); // Make cache alignment.

bool ToftThreadPool::ThreadContext::GetPendingTask() {
    std::unique_lock<std::mutex> guard(mutex);
    while (pending_tasks.empty() && !exit) {
        cond.wait(guard, [&]() { return !pending_tasks.empty() || exit; });
    }
    std::swap(pending_tasks, working_tasks);
    return !exit;
}

void ToftThreadPool::WorkRoutine(ToftThreadPool::ThreadContext *context) {
    while (true) {
        bool continued = context->GetPendingTask();
        for (auto &&task: context->working_tasks) {
            task();
        }
        context->working_tasks.clear();
        if (!continued) {
            break;
        }
    }

    std::unique_lock<std::mutex> guard(m_exit_lock);
    if (--m_num_busy_threads == 0) {
        m_exit_cond.notify_all();
    }
}

ToftThreadPool::ToftThreadPool(int num_threads) : m_num_threads(num_threads), m_num_busy_threads(0), m_exit(false) {
    if (num_threads <= 0) {
        m_num_threads = std::thread::hardware_concurrency();
    }
    m_thread_contexts = new ThreadContext[m_num_threads];
    for (size_t i = 0; i < m_num_threads; ++i) {
        m_thread_contexts[i].thread = std::make_unique<std::thread>(
                std::bind(&ToftThreadPool::WorkRoutine, this, &m_thread_contexts[i]));
    }
    m_num_busy_threads = m_num_threads;
}

bool ToftThreadPool::AnyTaskPending() const {
    for (size_t i = 0; i < m_num_threads; ++i) {
        std::unique_lock<std::mutex> guard(m_thread_contexts[i].mutex);
        if (!m_thread_contexts[i].pending_tasks.empty()) {
            return true;
        }
    }
    return false;
}

void ToftThreadPool::WaitForIdle() {
    if (!m_exit) {
        return;
    }
    while (AnyTaskPending()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

bool ToftThreadPool::AnyThreadRunning() const {
    return m_num_busy_threads > 0;
}


void ToftThreadPool::Terminate() {
    std::unique_lock<std::mutex> lock(m_exit_lock);
    if (m_exit) return;
    m_exit = true;
    for (size_t i = 0; i < m_num_threads; ++i) {
        std::unique_lock<std::mutex> thead_guard(m_thread_contexts[i].mutex);
        m_thread_contexts[i].exit = true;
        m_thread_contexts[i].cond.notify_all();
    }
    m_exit_cond.wait(lock, [&]() { return m_num_busy_threads == 0; });

    for (size_t i = 0; i < m_num_threads; ++i) {
        m_thread_contexts[i].thread->join();
    }
    delete[] m_thread_contexts;
    m_num_threads = 0;
}

ToftThreadPool::~ToftThreadPool() {
    Terminate();
}

void ToftThreadPool::AddTaskInternal(const ToftThreadPool::TaskFunc &function, size_t dispatch_key) {
    if (m_exit) return;
    auto &context = m_thread_contexts[dispatch_key % m_num_threads];
    {
        std::unique_lock<std::mutex> lock(context.mutex);
        context.pending_tasks.push_back(function);
        context.cond.notify_one();
    }
}

void ToftThreadPool::AddTaskInternal(ToftThreadPool::TaskFunc&& function, size_t dispatch_key) {
    if (m_exit) return;
    auto &context = m_thread_contexts[dispatch_key % m_num_threads];
    {
        std::unique_lock<std::mutex> lock(context.mutex);
        context.pending_tasks.push_back(std::forward<TaskFunc>(function));
        context.cond.notify_one();
    }
}


void ToftThreadPool::AddTask(const ToftThreadPool::TaskFunc &callback, size_t dispatch_key) {
    AddTaskInternal(callback, dispatch_key);
}

void ToftThreadPool::AddTask(const ToftThreadPool::TaskFunc &callback) {
    /// just use random number as a dispatch key
    AddTaskInternal(callback, GenRandom<size_t>());
}
