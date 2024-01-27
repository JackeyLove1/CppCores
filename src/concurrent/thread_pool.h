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
#include <list>

#include "utils/utils.h"

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
    std::condition_variable_any condition;
    std::atomic<bool> stop;
};

/*
 * @brief: a common thread task wrapper
 */

struct ThreadTask {
    using TaskFunc = std::function<void()>;

    explicit ThreadTask(TaskFunc func) :
            m_func(std::move(func)), m_hash_code(GenRandom<size_t>()) {};

    void operator()() const { m_func(); }

    struct Hash {
        size_t operator()(ThreadTask const &t) const noexcept { return t.m_hash_code; }
    };

    bool operator==(const ThreadTask &other) const noexcept {
        return m_hash_code == other.m_hash_code;
    }

    TaskFunc m_func;
    size_t m_hash_code;
};

/*
 * @improve:
 * 1. use LockFreeQueue as global task pool
 */
class ToftThreadPool {
public:
    using TaskFunc = std::function<void()>;
    using TaskList = std::list<TaskFunc>;
    using TaskNode = std::list<TaskFunc>::iterator;

    /// @param mun_threads number of threads, -1 means cpu number
    explicit ToftThreadPool(int num_threads = -1);

    ~ToftThreadPool();

    ToftThreadPool(const ToftThreadPool &) = delete;

    ToftThreadPool &operator=(const ToftThreadPool &) = delete;

    void AddTask(const TaskFunc &callback);


    void AddTask(const TaskFunc &callback, size_t dispatch_key);

    void WaitForIdle();

    void Terminate();

private:
    struct Task;
    struct ThreadContext;

    void AddTaskInternal(const TaskFunc &function, size_t dispatch_key);

    void AddTaskInternal(TaskFunc &&function, size_t dispatch_key);

    bool AnyTaskPending() const;

    void WorkRoutine(ThreadContext *thread);

    bool AnyThreadRunning() const;

private:
    ThreadContext *m_thread_contexts;
    size_t m_num_threads;
    size_t m_num_busy_threads;
    std::mutex m_exit_lock;
    std::condition_variable_any m_exit_cond;
    std::atomic<bool> m_exit;
};