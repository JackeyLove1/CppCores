#include <stdexcept>
#include <string>
#include <cstring>
#include <cerrno>
#include <cassert>
#include <pthread.h>

#include "base_thread.h"
#include "this_thread.h"
#include "posix_atomic.h"

static inline void CheckErrCode(int errcode, std::string msg) {
    if (errcode != 0) {
        throw std::runtime_error(std::move(msg) + std::string(strerror(errno)));
    }
}

ThreadAttributes &ThreadAttributes::SetName(std::string name) {
    m_name = std::move(name);
    return *this;
}

ThreadAttributes::ThreadAttributes() {
    CheckErrCode(pthread_attr_init(&m_attr), "Failed to init thread attr");
}

ThreadAttributes::~ThreadAttributes() {
    CheckErrCode(pthread_attr_destroy(&m_attr), "Failed to destory thread attr");
}

ThreadAttributes &ThreadAttributes::SetStackSize(size_t size) {
    CheckErrCode(pthread_attr_setstacksize(&m_attr, size), "Failed to set statck size");
    return *this;
}

ThreadAttributes &ThreadAttributes::SetDetached(bool detached) {
    int state = detached ? PTHREAD_CREATE_DETACHED : PTHREAD_CREATE_JOINABLE;
    CheckErrCode(pthread_attr_setdetachstate(&m_attr, state), "Failed to set thread detached");
    return *this;
}

ThreadAttributes &ThreadAttributes::SetPriority(int priority) {
    return *this;
}

bool ThreadAttributes::IsDetached() const {
    int state = 0;
    CheckErrCode(pthread_attr_getdetachstate(&m_attr, &state), "Failed to get thread attr");
    return state == PTHREAD_CREATE_DETACHED;
}

BaseThread::BaseThread() :
        m_handle(),
        m_id(-1),
        m_is_alive(false) {
}

BaseThread::BaseThread(const ThreadAttributes &attributes) :
        m_attributes(attributes),
        m_handle(),
        m_id(-1),
        m_is_alive(false) {
}

int BaseThread::DoStart() {
    // Start can only be used on a new born or a started but already joined
    // or a detached Thread object. In other words, not associated to any
    // system thread, both alive and dead.
    if (IsJoinable()) {
        // If crash here, means the thread is still alive or finished but not
        // joined.
        CheckErrCode(EINVAL, "Failed to start thread");
    }

    m_handle = ThreadHandleType();
    m_id = 0;

    const pthread_attr_t &attr = m_attributes.m_attr;
    int error = pthread_create(&m_handle, &attr, StaticEntry, this);
    if (error)
        m_id = -1;
    return error;
}

bool BaseThread::DoDetach() {
    CheckErrCode(pthread_detach(m_handle), "Failed to detach thread");
    m_handle = HandleType();
    m_id = -1;
    return true;
}

bool BaseThread::Join() {
    assert(IsJoinable());
    CheckErrCode(pthread_join(m_handle, nullptr), "Failed to join the thread");
    m_handle = pthread_t();
    m_id = -1;
    return true;
}

int BaseThread::GetId() const {
    if (m_id != 0)
        return m_id;

    // GetId is rarely used, so busy wait is more fitness
    while (AtomicGet(&m_id) == 0)
        ThisThread::Sleep(1);

    return m_id;
}

bool BaseThread::IsAlive() const {
    return m_is_alive;
}


bool BaseThread::IsJoinable() const {
    return !pthread_equal(m_handle, pthread_t());
}

// make sure execute before exit
void BaseThread::Cleanup(void *param) {
    BaseThread *thread = static_cast<BaseThread *>(param);
    thread->m_is_alive = false;
    thread->OnExit();
}

void BaseThread::OnExit() {
}

void *BaseThread::StaticEntry(void *param) {
    BaseThread *base_thread = static_cast<BaseThread *>(param);
    bool detached = base_thread->m_attributes.IsDetached();
    if (!detached) {
        base_thread->m_is_alive = true;
        base_thread->m_id = ThisThread::GetId();
    }
    const std::string &name = base_thread->m_attributes.m_name;
    if (!name.empty()) {
        // Set thread name for easy debugging.
#if __GLIBC__ > 2 || __GLIBC__ == 2 && __GLIBC_MINOR__ >= 12
        pthread_setname_np(pthread_self(), name.c_str());
#else
        prctl(PR_SET_NAME, name.c_str(), 0, 0, 0);
#endif
    }
    pthread_cleanup_push(Cleanup, param);
        base_thread->Entry();
        base_thread->m_is_alive = false;
    pthread_cleanup_pop(true);
}