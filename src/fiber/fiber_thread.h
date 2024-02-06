#pragma once

#include <memory>
#include <mutex>
#include <atomic>
#include <functional>
#include <string>
#include <string_view>

#include <pthread.h>

#include "fiber_singleton.h"
#include "fiber_nocopyable.h"
#include "fiber_mutex.h"

class FiberThread : public FiberNoncopyable {
public:
    typedef std::shared_ptr<FiberThread> ptr;
    typedef std::function<void()> Callback;

    explicit FiberThread(Callback cb, const std::string &name);

    ~FiberThread();

    pid_t getId() const noexcept { return m_id; }

    std::string_view getName() const noexcept { return m_name; }

    void join();

    static FiberThread *GetThis();

    static std::string_view GetName();

    static void SetName(const std::string &name);

private:
    static void *run(void *arg);

private:
    pid_t m_id{-1};
    pthread_t m_thread{0};
    Callback m_cb;
    std::string_view m_name;
    Semaphore m_semaphore;
};