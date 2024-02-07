
#pragma once
#include <memory>
#include <functional>
#include <ucontext.h>
#include <glog/logging.h>

class Scheduler;

class Fiber : public std::enable_shared_from_this<Fiber> {
    friend class Scheduler;

public:
    typedef std::shared_ptr<Fiber> ptr;
    typedef std::function<void()> Callback;
    /*
     * @brief fiber state
     */
    enum State {
        INIT,
        HOLD,
        EXEC,
        TERM,
        READY,
        EXCEPT,
    };

private:
    Fiber();

public:
    Fiber(Callback cb, size_t stack_size = 0, bool use_caller = false);

    ~Fiber();

    /*
     * @brief:
     * @pre getState() 为 INIT, TERM, EXCEPT
     * @post getState() = INIT
     */
    void reset(Callback cb);

    /**
     * @brief switch the fiber state to running
     * @pre getState() != EXEC
     * @post getState() = EXEC
     */
    void swapIn();

    /**
     * @brief switch fiber to background
     */
    void swapOut();

    /**
     * @brief switch the fiber state to running
     * @pre the main fiber call it to resume other fiber
     */
    void call();

    /**
     * @brief switch the fiber to background
     * @pre the worker fiber call it to main fiber
     * @post main fiber
     */
    void back();

    /**
     * @brief return fiber id
     */
    uint64_t getId() const { return m_id; }

    /**
     * @brief return fiber state
     */
    State getState() const { return m_state;}

public:
    /**
    * @brief Set the fiber to the thread
    * @param[in] f the fiber
    */
    static void SetThis(Fiber* f);

    /**
     * @brief return the current fiber
     */
    static Fiber::ptr GetThis();

    /**
     * @brief switch the fiber to background and set state to ready
     * @post getState() = READY
     */
    static void YieldToReady();

    /**
     * @brief switch the fiber to background and set state to hold
     * @post getState() = HOLD
     */
    static void YieldToHold();

    /**
     * @brief switch the fiber to background and set state to hold
     * @post getState() = HOLD
     */
    static void YieldToTerm();

    /**
     * @brief return total fibers
     */
    static uint64_t TotalFibers();

    /**
     * @brief main fiber
     * @post main fiber
     */
    static void MainFunc();

    /**
     * @brief main fiber
     * @post scheduler fiber
     */
    static void CallerMainFunc();

    /**
     * @brief get current fiber id
     */
    static uint64_t GetFiberId();

private:
    /// fiber id
    uint64_t m_id = 0;
    /// fiber stack
    uint32_t m_stacksize = 0;
    /// fiber state
    State m_state = INIT;
    /// fiber context
    ucontext_t m_ctx;
    /// fiber stack pointer
    void *m_stack = nullptr;
    /// fiber callback
    Callback m_cb;

};