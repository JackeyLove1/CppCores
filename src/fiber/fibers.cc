#include <atomic>
#include <mutex>
#include <numeric>
#include <ucontext.h>
#include <glog/logging.h>

#include "fiber_macros.h"
#include "fiber_flags.h"
#include "fibers.h"

static std::atomic<uint64_t> s_fiber_id{0};
static std::atomic<uint64_t> s_fiber_count{0};

/// current running fiber
static thread_local Fiber *t_fiber = nullptr;
/// main fiber, used to switch to main context
static thread_local Fiber::ptr t_threadFiber = nullptr;

/// TODO(JackyFan): use better memory pool
class MallocStackAllocator {
public:
    static void *Alloc(size_t size) {
        return malloc(size);
    }

    static void Dealloc(void *vp, size_t size) {
        return free(vp);
    }
};

using StackAllocator = MallocStackAllocator;

uint64_t Fiber::GetFiberId() {
    if (t_fiber) {
        return t_fiber->m_id;
    }
    return 0;
}

Fiber::Fiber() {
    m_state = EXEC;
    SetThis(this);
    if (getcontext(&m_ctx)) {
        FIBER_ASSERT_MSG(false, "getcontext");
    }
    s_fiber_count.fetch_and(1, std::memory_order_relaxed);
    LOG(INFO) << "Fiber::Fiber main";
}

Fiber::Fiber(Fiber::Callback cb, size_t stack_size, bool use_caller) :
        m_id(++s_fiber_id), m_cb(std::move(cb)) {
    s_fiber_count.fetch_add(1, std::memory_order_relaxed);
    m_stacksize = FLAGS_fiber_stack_size;
    m_stack = StackAllocator::Alloc(m_stacksize);
    if (m_stack == nullptr) {
        FIBER_ASSERT_MSG(false, "Allocate memory failed, id: " << m_id);
    }
    if (getcontext(&m_ctx)) {
        FIBER_ASSERT_MSG(false, "getcontext");
    }
    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;
    if (!use_caller) {
        makecontext(&m_ctx, &Fiber::MainFunc, 0);
    } else {
        makecontext(&m_ctx, &Fiber::CallerMainFunc, 0);
    }
    LOG(INFO) << "Succeed to init Fiber, id: " << m_id;
}

Fiber::~Fiber() {
    s_fiber_count.fetch_sub(1, std::memory_order_relaxed);
    if (m_stack) {
        FIBER_ASSERT(m_state == TERM || m_state == EXCEPT || m_state == INIT);
        StackAllocator::Dealloc(m_stack, m_stacksize);
    } else {
        FIBER_ASSERT(m_cb != nullptr);
        FIBER_ASSERT(m_state == EXEC);
        Fiber *cur = t_fiber;
        if (cur == this) {
            SetThis(nullptr);
        }
    }
    LOG(INFO) << "deconstruct fiber, id: " << m_id;
}

void Fiber::reset(Fiber::Callback cb) {
    FIBER_ASSERT(m_stack != nullptr);
    FIBER_ASSERT(m_state == TERM || m_state == INIT || m_state == EXCEPT);
    m_cb = std::move(cb);
    if (getcontext(&m_ctx)) {
        FIBER_ASSERT_MSG(false, "getcontext");
    }
    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    makecontext(&m_ctx, &Fiber::MainFunc, 0);
    m_state = INIT;
}

void Fiber::call() {
    SetThis(this);
    m_state = EXEC;
    if (swapcontext(&t_threadFiber->m_ctx, &m_ctx)) {
        FIBER_ASSERT_MSG(false, "swapcontext")
    }
}

void Fiber::back() {
    SetThis(t_threadFiber.get());
    if (swapcontext(&m_ctx, &t_threadFiber->m_ctx)) {
        FIBER_ASSERT_MSG(false, "swapcontext")
    }
}

void Fiber::swapIn() {
    //TODO(JackyFan): switch by schedule
}

void Fiber::swapOut() {
    //TODO(JackyFan): switch by schedule
}

void Fiber::SetThis(Fiber *f) {
    t_fiber = f;
}

Fiber::ptr Fiber::GetThis() {
    if (t_fiber) {
        return t_fiber->shared_from_this();
    }
    Fiber::ptr main_fiber(new Fiber);
    t_threadFiber = main_fiber;
    return t_fiber->shared_from_this();
}

void Fiber::YieldToReady() {
    auto cur = GetThis();
    FIBER_ASSERT(cur->m_state == EXEC);
    cur->m_state = READY;
    cur->swapOut();
}

void Fiber::YieldToHold() {
    auto cur = GetThis();
    FIBER_ASSERT(cur->m_state == EXEC);
    cur->m_state = HOLD;
    cur->swapOut();
}

void Fiber::YieldToTerm()  {
    auto cur = GetThis();
    FIBER_ASSERT(cur->m_state != TERM && cur->m_state != EXCEPT);
    cur->m_state = TERM;
    cur->swapOut();
}

uint64_t Fiber::TotalFibers() {
    return s_fiber_count.load(std::memory_order_relaxed);
}

void Fiber::MainFunc() {
    auto cur = GetThis();
    FIBER_ASSERT(cur != nullptr);
    try {
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = TERM;
    } catch (std::exception &ex) {
        cur->m_state = EXCEPT;
        LOG(ERROR) << "Failed to exec fiber, Except: " << ex.what() << " fiber id: " << cur->getId() << " \nbacktrace: "
                   << FiberBacktraceToString();
    } catch (...) {
        cur->m_state = EXCEPT;
        LOG(ERROR) << "Failed to exec fiber, Except. " << " fiber id: " << cur->getId() << " \nbacktrace: "
                   << FiberBacktraceToString();
    }
    auto *raw_ptr = cur.get();
    cur.reset();
    raw_ptr->swapOut();
    FIBER_ASSERT_MSG(false, "never reach fiber_id=" + std::to_string(raw_ptr->getId()))
}

void Fiber::CallerMainFunc() {
    auto cur = GetThis();
    FIBER_ASSERT(cur != nullptr);
    try {
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = TERM;
    } catch (std::exception &ex) {
        cur->m_state = EXCEPT;
        LOG(ERROR) << "Failed to exec fiber, Except: " << ex.what() << " fiber id: " << cur->getId() << " \nbacktrace: "
                   << FiberBacktraceToString();
    } catch (...) {
        cur->m_state = EXCEPT;
        LOG(ERROR) << "Failed to exec fiber, Except. " << " fiber id: " << cur->getId() << " \nbacktrace: "
                   << FiberBacktraceToString();
    }
    auto *raw_ptr = cur.get();
    cur.reset();
    raw_ptr->swapOut();
    FIBER_ASSERT_MSG(false, "never reach fiber_id=" + std::to_string(raw_ptr->getId()))
}