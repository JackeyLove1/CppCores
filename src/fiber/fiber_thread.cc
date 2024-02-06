#include <thread>
#include <mutex>
#include <unistd.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <glog/logging.h>

#include "fiber_thread.h"

static thread_local FiberThread *t_thread = nullptr;
static thread_local std::string_view t_thread_name = "UNKNOWN";

FiberThread *FiberThread::GetThis() {
    return t_thread;
}

std::string_view FiberThread::GetName() {
    return t_thread_name;
}

void FiberThread::SetName(const std::string &name) {
    if (name.empty()) return;
    if (t_thread) {
        t_thread->m_name = name;
    }
    t_thread_name = name;
}

FiberThread::FiberThread(Callback cb, const std::string &name) : m_cb(std::move(cb)), m_name(name) {
    if (name.empty()) {
        m_name = "UNKNOWN";
    }
    int rt = pthread_create(&m_thread, nullptr, &FiberThread::run, this);
    if (rt) {
        LOG(FATAL) << "pthread_create thread fail, rt=" << rt << " name=" << name;
    }
    m_semaphore.wait();
}

FiberThread::~FiberThread() noexcept {
    if (m_thread) {
        pthread_detach(m_thread);
    }
}

void FiberThread::join() {
    if (m_thread) {
        int rt = pthread_join(m_thread, nullptr);
        if (rt) {
            LOG(FATAL) << "pthread_create thread fail, rt=" << rt << " name=" << m_name;
        }
        m_thread = 0;
    }
}

void *FiberThread::run(void *arg) {
    FiberThread *thread = static_cast<FiberThread *>(arg);
    t_thread = thread;
    t_thread_name = thread->m_name;
    thread->m_id = syscall(SYS_gettid);
    pthread_setname_np(pthread_self(), thread->m_name.substr(0, 15).data());

    std::function<void()> cb;
    cb.swap(t_thread->m_cb);
    t_thread->m_semaphore.notify();
    std::invoke(cb);
    return nullptr;
}